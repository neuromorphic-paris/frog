package com.example.chronocam.atis;

import android.graphics.Bitmap;
import android.util.Log;

import java.nio.ByteBuffer;

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
    private transient ByteBuffer byteBuffer;

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
        render_preview(this.byteBuffer);
    }

    void setBitmap(Bitmap bitmap){
        this.byteBuffer = set_bitmap(objPtr, bitmap);
    }

    String stringFromJNI(){
        return string_from_JNI();
    }

    void triggerSepia(String filepath){
        trigger_sepia(objPtr, byteBuffer, filepath);
    }

    private native long new_Eventprocessor();
    private native long delete_Eventprocessor(long jniCPtr);
    private native void set_camera_data_Eventprocessor(long jniCPtr, Eventprocessor eventprocessor, byte[] arg0, long arg1);
    private native void render_preview(ByteBuffer handle);
    private native ByteBuffer set_bitmap(long objPtr, Bitmap bitmap);
    private native String string_from_JNI();
    native int get_JVM_version();
    private native void trigger_sepia(long objPtr, ByteBuffer byteBuf, String path);
}
