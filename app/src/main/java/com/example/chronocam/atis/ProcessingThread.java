package com.example.chronocam.atis;

import android.os.HandlerThread;

/**
 * Created by gregorlenz on 28/12/17.
 */

class ProcessingThread extends HandlerThread {

    public ProcessingThread(String name) {
        super(name);
    }
}
