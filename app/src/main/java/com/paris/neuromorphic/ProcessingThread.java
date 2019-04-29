package com.paris.neuromorphic;

import android.os.HandlerThread;
import android.util.Log;

import com.esotericsoftware.kryo.Kryo;
import com.esotericsoftware.kryo.io.Output;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.text.DecimalFormat;
import java.util.concurrent.BlockingQueue;

/**
 * Created by Gregor on 12/12/2018.
 */

public class ProcessingThread extends HandlerThread {
    private String TAG = getClass().getSimpleName();

    private volatile boolean isCameraAttached = true;

    private ToExchange toExchange;
    private final BlockingQueue buffer;
    private Eventprocessor eventprocessor;

    private long startTimeStamp, currentTimeStamp, lastTimeStamp;

    private long iterationCounter;

    private Kryo kryo;
    private Output output;

    static final String filePath = "/data/user/0/com.vision.neuromorphic.frog/files/recording.bin";
    DecimalFormat df = new DecimalFormat("####.##");


    ProcessingThread(BlockingQueue blockingQueue) {
        super(ProcessingThread.class.getName());
        buffer = blockingQueue;
        eventprocessor = new Eventprocessor();
        kryo = new Kryo();
        kryo.register(ToExchange.class);
        kryo.register(byte[].class);
        Log.d(TAG, filePath);
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
        //try {
        //    output = new Output(new FileOutputStream(filePath));
        //} catch (FileNotFoundException e) {
        //    e.printStackTrace();
        //}
        while (isCameraAttached) {
            //saveBufferElement();
            processBufferElement();
        }
        //output.close();
        super.quit();
    }

    private void saveBufferElement() {
        try {
            toExchange = (ToExchange) buffer.take();
            kryo.writeObject(output, toExchange);
            Log.i(TAG, "Consumer: Saved Exchange to file with size " + toExchange.size
                    + ", remaining buffer capacity: " + buffer.remainingCapacity());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void processBufferElement() {
        try {
            toExchange = (ToExchange) buffer.take();
            startTimeStamp = System.nanoTime();
            eventprocessor.setCameraData(toExchange.data, toExchange.size);
            currentTimeStamp = System.nanoTime();

            iterationCounter++;
            Log.d(TAG, "Consumer: Processing Exchange no. " + iterationCounter + " with size " + toExchange.size + " took "
                    + df.format((currentTimeStamp - startTimeStamp) / 1000000f) + "ms, it's been "
                    + df.format((startTimeStamp - lastTimeStamp) / 1000000f) + "ms since last call, remaining buffer capacity: "
                    + buffer.remainingCapacity());
            lastTimeStamp = currentTimeStamp;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    void setCameraAttached(boolean flag) {
        isCameraAttached = flag;

        if (output != null) {
            output.close();
        }
    }

    @Override
    public boolean quit() {
        return super.quit();
    }

}