package com.paris.neuromorphic;

import android.os.HandlerThread;
import android.util.Log;

import java.util.concurrent.BlockingQueue;

/**
 * Created by Gregor on 12/12/2018.
 */

public class ProcessingThread extends HandlerThread{
    private String TAG = getClass().getName();

    private volatile boolean isCameraAttached = true;

    private final BlockingQueue buffer;
    private Eventprocessor eventprocessor;

    private int eventCount, maxCount, iterationCount;

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

        processExchange();
    }

    int globalSize=0;

    void processExchange() {
        while (isCameraAttached) {
            if (!buffer.isEmpty()) {
                processToExchange();

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

    void processToExchange() {
        try {
            iterationCount++;
            CameraPollingThread.EventExchange toExchange;
            toExchange = (CameraPollingThread.EventExchange) buffer.take();
            int size = toExchange.size;
            if (size == 16384) {
                maxCount++;
            }
            globalSize+=size;
            Log.d(TAG, "Consumer: Processing Exchange with size: " + size + ", remaining buffer capacity: " + buffer.remainingCapacity());
            eventCount += size;

            eventprocessor.setCameraData(toExchange.data, size);
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