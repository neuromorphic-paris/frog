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

    private static transient long objPtr = 0;

    static void newEventprocessor(){
        if (objPtr == 0) {
            try {
                objPtr = new_Eventprocessor();
            } catch (UnsatisfiedLinkError e) {
                objPtr = 0;
            }
        }
    }

    protected static long getCPtr(Eventprocessor obj) {
        return (obj == null) ? 0 : objPtr;
    }

    public synchronized void delete() {
        if (objPtr != 0) {
            delete_Eventprocessor(objPtr);
            objPtr = 0;
        }
    }

    static void setBitmap(Bitmap bitmap) {
        if (isEventProcessorLibraryLoaded()) {
            if (objPtr == 0) {
                newEventprocessor();
                Log.d(TAG, "Created new Eventprocessor from setBitmap");
            }
            set_bitmap(objPtr, bitmap);
        } else {
            Log.e(TAG, "libeventprocessor is not loaded");
        }
    }

    static void resetBitmap() {
        if (isEventProcessorLibraryLoaded()) {
            if (objPtr == 0) {
                newEventprocessor();
                Log.d(TAG, "Created new Eventprocessor from resetBitmap");
            }
            reset_bitmap(objPtr);
        } else {
            Log.e(TAG, "libeventprocessor is not loaded");
        }
    }

    static void deleteBitmap() {
        delete_bitmap(objPtr);
    }

    static void triggerSepia(String filepath) {
        trigger_sepia(objPtr, filepath);
    }

    static void setCameraData(byte[] data, long length) {
        set_camera_data(objPtr, data, length);
    }

    int getJvmVersion() {
        return get_JVM_version();
    }

    private static native long new_Eventprocessor();

    private static native void delete_Eventprocessor(long jniCPtr);

    private static native void set_bitmap(long objPtr, Bitmap bitmap);

    private static native void reset_bitmap(long objPtr);

    private static native void delete_bitmap(long objPtr);

    private static native void trigger_sepia(long objPtr, String path);

    private static native void set_camera_data(long objPtr, byte[] data, long length);

    private static native int get_JVM_version();

    // Robolectric cannot load shared libraries
    private static boolean isEventProcessorLibraryLoaded() {
        try {
            Set<String> libs = new HashSet<>();
            String mapsFile = "/proc/" + android.os.Process.myPid() + "/maps";
            BufferedReader reader = new BufferedReader(new FileReader(mapsFile));
            String line;
            while ((line = reader.readLine()) != null) {
                if (line.endsWith(".so")) {
                    int n = line.lastIndexOf(" ");
                    libs.add(line.substring(n + 1));
                }
            }
            for (String element:libs) {
                Log.d(TAG, element);
                if (element.contains("libeventprocessor.so")) {
                    return true;
                }
            }
            return false;
        } catch (FileNotFoundException e) {
            Log.e(TAG, "cannot check libraries, are you running unit tests?");
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

}
