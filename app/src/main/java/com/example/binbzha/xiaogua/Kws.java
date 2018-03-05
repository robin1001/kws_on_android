package com.example.binbzha.xiaogua;

/**
 * Created by binbzha on 8/12/2017.
 */

public class Kws {
    private native String Hello();
    private native void Init(String netFile, String cmvnFile, String vadNetFile, String vadCmvnFile);
    private native void Reset();

    static {
        System.loadLibrary("xiaogua");
    }

    void init(String netFile, String cmvnFile, String vadNetFile, String vadCmvnFile) {
        Init(netFile, cmvnFile, vadNetFile, vadCmvnFile);
    }

    void reset() {
        Reset();
    }
}
