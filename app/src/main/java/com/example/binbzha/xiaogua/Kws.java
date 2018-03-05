package com.example.binbzha.xiaogua;

/**
 * Created by binbzha on 8/12/2017.
 */

public class Kws {
    private native String Hello();
    private native void Init(String netFile, String cmvnFile, String fsmFile);
    private native boolean DetectOnline(short []pcm, boolean end);
    private native void Reset();

    static {
        System.loadLibrary("xiaogua");
    }

    void init(String netFile, String cmvnFile, String fsmFile) {
        Init(netFile, cmvnFile, fsmFile);
    }

    boolean detectOnline(short []pcm, boolean end) {
        return DetectOnline(pcm, end);
    }

    void reset() {
        Reset();
    }
}
