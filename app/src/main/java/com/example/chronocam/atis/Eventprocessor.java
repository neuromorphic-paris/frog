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
        }
    }
    private transient long objPtr;

    public Eventprocessor() {
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

    public void setCameraData(byte[] arg0, long arg1) {
        set_camera_data_Eventprocessor(objPtr, this, arg0, arg1);
    }

    void renderPreview(){
        render_preview(objPtr);
    }

    void setBitmap(Bitmap bitmap){
        set_bitmap(objPtr, bitmap);
    }

    String stringFromJNI(){
        return string_from_JNI();
    }

    void triggerSepia(String filepath){
        trigger_sepia(objPtr, filepath);
    }

    private native long new_Eventprocessor();
    private native long delete_Eventprocessor(long jniCPtr);
    private native void set_camera_data_Eventprocessor(long jniCPtr, Eventprocessor eventprocessor, byte[] arg0, long arg1);
    private native void render_preview(long objPtr);
    private native void set_bitmap(long objPtr, Bitmap bitmap);
    private native String string_from_JNI ();
    private native void trigger_sepia(long objPtr, String path);
}
