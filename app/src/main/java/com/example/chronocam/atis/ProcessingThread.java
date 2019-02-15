package com.example.chronocam.atis;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;

import java.util.concurrent.BlockingQueue;

/**
 * Created by Gregor on 12/12/2018.
 */

public class ProcessingThread extends HandlerThread{
    private String TAG = getClass().getName();
    //Camera dimensions
    private static final int WIDTH = 304;
    private static final int HEIGHT = 240;
    volatile boolean isCameraAttached = true;
    static Bitmap preview;

    private Intent intent;
    private final BlockingQueue buffer;
    Eventprocessor eventprocessor;

    private volatile boolean isRecording = true;
    private volatile boolean shouldPredict = false;

    private Handler resultHandler;

    private int eventCount, maxCount, iterationCount;


    public ProcessingThread(Intent intent, Handler handler, BlockingQueue blockingQueue) {
        super(ProcessingThread.class.getName());
        this.intent = intent;
        this.resultHandler = handler;
        buffer = blockingQueue;
        preview = Bitmap.createBitmap(WIDTH, HEIGHT, Bitmap.Config.ALPHA_8);
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
        String[] paths;
        paths = intent.getStringArrayExtra("filePath");
        //eventprocessor = new Eventprocessor();
        //eventprocessor.init(paths[1], paths[2], paths[3]);

        while (isCameraAttached) {
            if (!buffer.isEmpty()) {
                processToExchange();

            } else {
                try {
                    Thread.sleep(15);
                    Log.d(TAG, "Just resting my eyes...");
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

            //eventprocessor.set_camera_data(toExchange.data, size);
            process_data_raw(toExchange);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }


    void process_data_raw(CameraPollingThread.EventExchange toExchange) {
        long size = toExchange.size;
        for (int i = 0; i <= size; )
            processGrade(toExchange.data[i++], toExchange.data[i++], toExchange.data[i++], toExchange.data[i++]);
    }

    //TODO decode timestamp here
    private void processGrade(byte a, byte b, byte c, byte d) {
        byte p = (byte) ((d & 0xf0) >> 4);
        if (p == 8) {
            //Log.d("processGrade", Integer.toString((((a & 0xff) | ((b & 0xff) << 8) | ((c & 0xff) << 16) | ((d & 0x0f) << 24)) << 11)));
            return;
        }
        int y = a & 0xff;
        if (y < 0 || y >= HEIGHT)
            return;
        int x = (b & 0xff) + ((c & 0x01) << 8);
        if (x < 0 || x >= WIDTH)
            return;
        y = 239 - y;
        if (p > 0) {
            preview.setPixel(x, y, Color.BLACK);
        } else {
            preview.setPixel(x, y, Color.TRANSPARENT);
        }
        //bytePreview[x][y] = p;

        eventCount++;
        if (eventCount % 10000 == 0) {
            Log.d(TAG, "10000 events received");
            eventCount = 0;
        }
    }

    /*
     *Setters
     */
    void setCameraAttached(boolean flag) {
        isCameraAttached = flag;
    }

    void setRecordingStatus(boolean flag) {
        isRecording = flag;
    }

    @Override
    public boolean quit() {
        return super.quit();
    }

}