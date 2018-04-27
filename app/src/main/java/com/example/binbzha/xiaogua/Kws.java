package com.example.binbzha.xiaogua;

/**
 * Created by binbzha on 8/12/2017.
 */

public class Kws {
    private native String Hello();
    private native void Init(String netFile, String cmvnFile, String fstFile, String fillerFile);
    private native Status DetectOnline(short []pcm, boolean end);
    private native void Reset();
    private native void SetThresh(float thresh);

    static {
        System.loadLibrary("xiaogua");
    }

    public void init(String netFile, String cmvnFile, String fstFile, String fillerFile) {
        Init(netFile, cmvnFile, fstFile, fillerFile);
    }

    public Status detectOnline(short []pcm, boolean end) {
        return DetectOnline(pcm, end);
    }

    public void reset() {
        Reset();
    }

    public String hello() { return Hello(); }

    public void setThresh(float thresh) {
        SetThresh(thresh);
    }
}
