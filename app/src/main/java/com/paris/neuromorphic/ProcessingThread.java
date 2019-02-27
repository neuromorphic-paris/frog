package com.paris.neuromorphic;

import android.os.HandlerThread;
import android.util.Log;

import java.util.concurrent.BlockingQueue;

/**
 * Created by Gregor on 12/12/2018.
 */

public class ProcessingThread extends HandlerThread{
    private String TAG = getClass().getSimpleName();

    private volatile boolean isCameraAttached = true;

    private CameraPollingThread.EventExchange toExchange;
    private final BlockingQueue buffer;
    private Eventprocessor eventprocessor;

    private long startTimeStamp, currentTimeStamp;

    ProcessingThread(BlockingQueue blockingQueue) {
        super(ProcessingThread.class.getName());
        buffer = blockingQueue;
        eventprocessor = new Eventprocessor();
    }

    @Override
    public void run() {
        Log.d(TAG, "Consumer run method");
        super.run();
    }

    @Override
    protected void onLooperPrepared() {
        Log.d(TAG, "Looper prepared");
        super.onLooperPrepared();

        checkBufferForNewElements();
    }

    private void checkBufferForNewElements() {
        while (isCameraAttached) {
            if (!buffer.isEmpty()) {
                processBufferElement();
            } else {
                try {
                    Thread.sleep(10);
                    //Log.d(TAG, "Just resting my eyes...");
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private void processBufferElement() {
        try {
            toExchange = (CameraPollingThread.EventExchange) buffer.take();
            startTimeStamp = System.nanoTime();
            eventprocessor.setCameraData(toExchange.data, toExchange.size);
            currentTimeStamp = System.nanoTime();

            Log.d(TAG, "Consumer: Processing Exchange with size " + toExchange.size + " took "
                    + (currentTimeStamp - startTimeStamp)/100000 + "ms, remaining buffer capacity: " + buffer.remainingCapacity());

        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    void setCameraAttached(boolean flag) {
        isCameraAttached = flag;
    }

    @Override
    public boolean quit() {
        return super.quit();
    }

}