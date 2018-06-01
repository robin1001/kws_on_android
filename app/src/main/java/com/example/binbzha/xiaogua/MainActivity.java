package com.example.binbzha.xiaogua;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Environment;
import android.os.Process;
import android.support.v4.view.GestureDetectorCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {
    final String LOG_TAG = "Xiaogua";
    final int SAMPLE_RATE = 16000; // The sampling rate
    int miniBufferSize = 0; // 1280 bytes 648 byte 0.04ms
    final int MAX_QUEUE_SIZE = 250; // 100 seconds audio, 1 / 0.04 * 100
    Queue<short[]> bufferQueue = new LinkedList<short[]>();

    AudioRecord record = null;
    TextView textView = null;
    TextView tipTextView = null;
    SeekBar seekBar = null;
    GestureDetectorCompat gestureDetector = null;
    VoiceRectView voiceView = null;
    static Object lockRecord = new Object();
    static Object queueLock = new Object();

    Kws kws = new Kws();
    Status status = null;

    private final String wavFile =  Environment.getExternalStorageDirectory() + "/register.wav";
    private String netFile = "data/data/com.example.binbzha.xiaogua/kws.net";
    private String cmvnFile = "data/data/com.example.binbzha.xiaogua/kws.cmvn";
    private String fstFile = "data/data/com.example.binbzha.xiaogua/kws.fst";
    private String fillerFile = "data/data/com.example.binbzha.xiaogua/kws.filler";
    private final int kMax = 100;
    private float confidence = 0.2f;
    private Hashtable<Integer, String> keywordTable = new Hashtable<Integer, String>();
    private Hashtable<Integer, Float> confidenceTable = new Hashtable<Integer, Float>();
    // keep same with endpoint_thresh in xiaogua.cc
    private int endpointLength = 6400; // 0.5 * 16000
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView = (TextView)findViewById(R.id.text_view);
        tipTextView =(TextView)findViewById(R.id.tip_text_view);
        voiceView = (VoiceRectView)(findViewById(R.id.voice_rect_view));
        seekBar = (SeekBar)(findViewById(R.id.seek_bar));
        seekBar.setMax(kMax);
        seekBar.setProgress((int)(confidence * kMax));
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                confidence = (float)progress / kMax;
                textView.setText(String.format("threshold %.2f", confidence));
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                //kws.setThresh(confidence);
            }
        });
        seekBar.setVisibility(View.INVISIBLE);
        gestureDetector = new GestureDetectorCompat(this, new MyGestureListener());

        // Keyword spotting related initialization
        keywordTable.put(1, "你好小瓜");
        confidenceTable.put(1, 0.1f);
        String tipString = "Speak the following words to trigger\n";
        Enumeration keys = keywordTable.keys();
        while (keys.hasMoreElements()) {
            Integer key = (Integer)keys.nextElement();
            tipString = tipString + "\n" + keywordTable.get(key);
        }
        tipTextView.setText(tipString);

        copyDataFile();
        textView.setText(kws.hello());
        kws.init(netFile, cmvnFile, fstFile, fillerFile);
        kws.setThresh(confidence);
        initRecoder();
        startRecordThread();
        startKwsThread();
    }

    public boolean onTouchEvent(MotionEvent event){
        gestureDetector.onTouchEvent(event);
        return super.onTouchEvent(event);
    }

    class MyGestureListener extends GestureDetector.SimpleOnGestureListener {
        private static final String DEBUG_TAG = "Gestures";
        @Override
        public boolean onDoubleTap(MotionEvent event) {
            if (seekBar.getVisibility() == View.VISIBLE) {
                seekBar.setVisibility(View.INVISIBLE);
            } else {
                seekBar.setVisibility(View.VISIBLE);
            }
            return true;
        }
    }

    void initRecoder () {
        // buffer size in bytes 1280
        miniBufferSize = AudioRecord.getMinBufferSize(SAMPLE_RATE,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT);
        if (miniBufferSize == AudioRecord.ERROR || miniBufferSize == AudioRecord.ERROR_BAD_VALUE) {
            Log.e(LOG_TAG, "Audio buffer can't initialize!");
            return;
        }
        record = new AudioRecord(MediaRecorder.AudioSource.DEFAULT,
                SAMPLE_RATE,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT,
                miniBufferSize);
        if (record.getState() != AudioRecord.STATE_INITIALIZED) {
            Log.e(LOG_TAG, "Audio Record can't initialize!");
            return;
        }

        record.startRecording();
    }

    double calculateDb(short[] buffer, int offset, int length) {
        assert(offset + length < buffer.length);
        double energy = 0.0;
        for (int i = 0; i < length; i++) {
            energy += buffer[offset + i] * buffer[offset + i];
        }
        energy /= length;
        energy = (10 * Math.log10(1 + energy)) / 140;
        energy = Math.min(energy, 1.0);
        return energy;
    }

    short [] getChunkFromQueueBuffer() {
        short [] buffer = null;
        synchronized (queueLock) {
            while (bufferQueue.size() == 0) {
                try {
                    queueLock.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            buffer = bufferQueue.poll();
            queueLock.notifyAll();
        }
        assert(buffer != null);
        return buffer;
    }

    void clearQueueBuffer() {
        synchronized (queueLock) {
            while (bufferQueue.size() > 0) {
                bufferQueue.poll();
            }
            queueLock.notifyAll();
        }
    }

    void startRecordThread() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                Process.setThreadPriority(Process.THREAD_PRIORITY_AUDIO);
                while (true) {
                    short [] buffer = new short[miniBufferSize / 2];
                    int points = record.read(buffer, 0, buffer.length);
                    voiceView.add(calculateDb(buffer, 0, points));

                    synchronized (queueLock) {
                        // bufferQueue is full, just drop first chunk
                        if (bufferQueue.size() == MAX_QUEUE_SIZE) {
                            //Log.w(LOG_TAG, "buffer queue is full, drop one");
                            bufferQueue.poll();
                        }
                        bufferQueue.offer(buffer);
                        queueLock.notifyAll();
                    }
                }
                //record.release();
            }
        }).start();
    }

    short [] convertListToArray(ArrayList<short[]> list) {
        int size = 0;
        for (short [] elem: list) size += elem.length;
        assert (size > 0);
        short [] pcm = new short[size];
        int offset = 0;
        for (short [] elem: list) {
            for (short x: elem) {
                pcm[offset++] = x;
            }
        }
        return pcm;
    }

    void startKwsThread() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                clearQueueBuffer();
                while (true) {
                    short [] buffer = getChunkFromQueueBuffer();
                    status = kws.detectOnline(buffer, false);
                    if (status.legal && status.confidence > 0.001) {
                        Log.w(LOG_TAG, String.format("Spotting: %f %d", status.confidence, status.keyword));
                        if (status.confidence > confidenceTable.get(status.keyword)) {
                            runOnUiThread(new Runnable() {
                                public void run() {
                                    textView.setText(String.format("%s %f %s",
                                            keywordTable.get(status.keyword),
                                            status.confidence,
                                            status.confidence > confidenceTable.get(status.keyword) ? "accepted" : "rejected"));
                                }
                            });

                            try {
                                Thread.sleep(1000);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            clearQueueBuffer();
                            runOnUiThread(new Runnable() {
                                public void run() {
                                    textView.setText("");
                                }
                            });
                            kws.reset();
                        }
                    }
                }
            }
        }).start();
    }

    private void copyDataFile() {
        try {
            copyBigDataTo("kws.net", netFile);
            copyBigDataTo("kws.cmvn", cmvnFile);
            copyBigDataTo("kws.fst", fstFile);
            copyBigDataTo("kws.filler", fillerFile);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void copyBigDataTo(String assetName, String strOutFileName) throws IOException
    {
        InputStream myInput;
        File file = new File(strOutFileName);
        OutputStream myOutput = new FileOutputStream(strOutFileName);
        myInput = this.getAssets().open(assetName);
        byte[] buffer = new byte[1024];
        int length = myInput.read(buffer);
        while(length > 0)
        {
            myOutput.write(buffer, 0, length);
            length = myInput.read(buffer);
        }
        myOutput.flush();
        myInput.close();
        myOutput.close();
    }
}
