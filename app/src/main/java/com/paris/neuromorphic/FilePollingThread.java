package com.paris.neuromorphic;

import android.content.Intent;
import android.os.HandlerThread;

import java.util.concurrent.BlockingQueue;

class FilePollingThread extends HandlerThread {
    private final String TAG = getClass().getSimpleName();
    private Intent intent;
    private final BlockingQueue buffer;

    FilePollingThread(Intent intent, BlockingQueue blockingQueue) {
        super(FilePollingThread.class.getName());
        this.intent = intent;
        this.buffer = blockingQueue;
    }
}
