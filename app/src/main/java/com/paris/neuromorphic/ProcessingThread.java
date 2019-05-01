package com.paris.neuromorphic;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;

import com.esotericsoftware.kryo.Kryo;
import com.esotericsoftware.kryo.io.Output;

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
    private Handler resultHandler;

    private long startTimeStamp, currentTimeStamp, lastTimeStamp;

    private long iterationCounter;

    private Kryo kryo;
    private Output output;

    static final String filePath = "/data/user/0/com.vision.neuromorphic.frog/files/recording.bin";
    DecimalFormat df = new DecimalFormat("####.##");


    ProcessingThread(BlockingQueue blockingQueue, Handler resultHandler) {
        super(ProcessingThread.class.getName());
        this.buffer = blockingQueue;
        this.resultHandler = resultHandler;
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

        //saveBufferElements();
        processBufferElements();
        super.quit();
    }

    private void saveBufferElements() {
        try {
            output = new Output(new FileOutputStream(filePath));
            while (isCameraAttached) {
                toExchange = (ToExchange) buffer.take();
                toExchange.isRecorded = true;
                kryo.writeObject(output, toExchange);
                Log.i(TAG, "Consumer: Saved Exchange to file with size " + toExchange.size
                        + ", remaining buffer capacity: " + buffer.remainingCapacity());
            }
            output.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void processBufferElements() {
        while (isCameraAttached) {
            try {
                toExchange = (ToExchange) buffer.take();
                if (toExchange.size == 13) {
                    String gesture = Eventprocessor.predict();
                    Message message = Message.obtain();
                    message.obj = gesture;
                    resultHandler.sendMessage(message);
                    Log.i(TAG, gesture);
                } else {
                    startTimeStamp = System.nanoTime();
                    Eventprocessor.setCameraData(toExchange.data, toExchange.size, toExchange.isRecorded);
                    currentTimeStamp = System.nanoTime();

                    iterationCounter++;
                /*
                Log.d(TAG, "Consumer: Processing Exchange no. " + iterationCounter + " with size " + toExchange.size + " took "
                        + df.format((currentTimeStamp - startTimeStamp) / 1000000f) + "ms, it's been "
                        + df.format((startTimeStamp - lastTimeStamp) / 1000000f) + "ms since last call, remaining buffer capacity: "
                        + buffer.remainingCapacity());
                        */
                    lastTimeStamp = currentTimeStamp;
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
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