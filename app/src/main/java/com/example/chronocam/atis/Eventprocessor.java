package com.example.chronocam.atis;

import android.graphics.Bitmap;
import android.util.Log;

public class Eventprocessor {
    private static final String TAG = Eventprocessor.class.getName();
    static {
        try {
            System.loadLibrary("eventprocessor");
        } catch (final UnsatisfiedLinkError e){
            Log.e(TAG, "load library" + Log.getStackTraceString(e));
        }    }
    private transient long objPtr;

    public Eventprocessor() {
        if (this.objPtr != 0) {
            try {
                this.objPtr = new_Eventprocessor();
            } catch (UnsatisfiedLinkError e) {
                this.objPtr = 0;
            }
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

    public void set_camera_data(byte[] arg0, long arg1) {
        set_camera_data_Eventprocessor(objPtr, this, arg0, arg1);
    }

    private native long new_Eventprocessor();
    private native long delete_Eventprocessor(long jniCPtr);
    private native void set_camera_data_Eventprocessor(long jniCPtr, Eventprocessor eventprocessor, byte[] arg0, long arg1);

    public native void renderPreview(Bitmap bitmap);
    public native String stringFromJNI ();
    public native void triggerSepia(String path);
}
