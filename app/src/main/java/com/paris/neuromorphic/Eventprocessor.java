package com.paris.neuromorphic;

import android.graphics.Bitmap;
import android.util.Log;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashSet;
import java.util.Set;

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
        if (isEventProcessorLibraryLoaded()) {
            set_bitmap(objPtr, bitmap);
        } else {
            Log.e(TAG, "libeventprocessor is not loaded");
        }
    }

    void resetBitmap() {
        if (isEventProcessorLibraryLoaded()) {
            reset_bitmap(objPtr);
        } else {
            Log.e(TAG, "libeventprocessor is not loaded");
        }
    }

    void deleteBitmap() {
        delete_bitmap();
    }

    void triggerSepia(String filepath) {
        trigger_sepia(objPtr, filepath);
    }

    void setCameraData(byte[] data, long length) {
        set_camera_data(objPtr, data, length);
    }

    int getJvmVersion() {
        return get_JVM_version();
    }

    private native long new_Eventprocessor();

    private native void delete_Eventprocessor(long jniCPtr);

    private native void set_bitmap(long objPtr, Bitmap bitmap);

    private native void reset_bitmap(long objPtr);

    private native void delete_bitmap();

    private native void trigger_sepia(long objPtr, String path);

    private native void set_camera_data(long objPtr, byte[] data, long length);

    native int get_JVM_version();

    // Robolectric cannot load shared libraries
    boolean isEventProcessorLibraryLoaded() {
        try {
            Set<String> libs = new HashSet<String>();
            String mapsFile = "/proc/" + android.os.Process.myPid() + "/maps";
            BufferedReader reader = new BufferedReader(new FileReader(mapsFile));
            String line;
            while ((line = reader.readLine()) != null) {
                if (line.endsWith(".so")) {
                    int n = line.lastIndexOf(" ");
                    libs.add(line.substring(n + 1));
                }
            }
            return libs.contains("/data/app/com.vision.neuromorphic.frog-2/lib/arm64/libeventprocessor.so")
                    || libs.contains("set/data/app/com.vision.neuromorphic.frog/lib/arm64/libeventprocessor.so;");
        } catch (FileNotFoundException e) {
            // Do some error handling...
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

}
