package com.example.chronocam.atis;

import android.os.HandlerThread;

/**
 * Created by gregorlenz on 26/12/17.
 */

public class CameraPollingThread extends HandlerThread {

    public CameraPollingThread(String name) {
        super(name);
    }
}
