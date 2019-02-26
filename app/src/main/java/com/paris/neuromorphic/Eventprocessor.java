package com.paris.neuromorphic;

import android.graphics.Bitmap;
import android.util.Log;

public class Eventprocessor {
    private static final String TAG = Eventprocessor.class.getName();

    static {
        try {
            System.loadLibrary("eventprocessor");
        } catch (final UnsatisfiedLinkError e) {
            Log.e(TAG, "load library" + Log.getStackTraceString(e));
        }
    }

    private transient long objPtr;

    Eventprocessor() {
        try {
            this.objPtr = new_Eventprocessor();
        } catch (UnsatisfiedLinkError e) {
            this.objPtr = 0;
        }
    }

    protected static long getCPtr(Eventprocessor obj) {
        return (obj == null) ? 0 : obj.objPtr;
    }

    public synchronized void delete() {
        if (objPtr != 0) {
            delete_Eventprocessor(objPtr);
            objPtr = 0;
        }
    }

    void setBitmap(Bitmap bitmap) {
        set_bitmap(objPtr, bitmap);
    }

    void resetBitmap() {
        reset_bitmap(objPtr);
    }

    void deleteBitmap() {
        delete_bitmap(objPtr);
    }

    void triggerSepia(String filepath) {
        trigger_sepia(objPtr, filepath);
    }

    void setCameraData(byte[] data, long length){
        set_camera_data(objPtr, data, length);
    }

    int getJvmVersion() {
        return get_JVM_version();
    }

    private native long new_Eventprocessor();

    private native void delete_Eventprocessor(long jniCPtr);

    private native void set_bitmap(long objPtr, Bitmap bitmap);

    private native void reset_bitmap(long objPtr);

    private native void delete_bitmap(long objPtr);

    private native void trigger_sepia(long objPtr, String path);

    private native void set_camera_data(long objPtr, byte[] data, long length);

    native int get_JVM_version();
}
