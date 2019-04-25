package com.paris.neuromorphic;

import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbManager;
import android.os.Binder;
import android.os.IBinder;
import android.os.Looper;
import android.util.Log;
import android.widget.Toast;

import com.esotericsoftware.kryo.Kryo;
import com.esotericsoftware.kryo.io.Input;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.concurrent.ArrayBlockingQueue;

import androidx.annotation.Nullable;

/**
 * Created by gregorlenz on 31/10/18.
 */

public class CameraService extends Service {
    private final String TAG = getClass().getName();
    private final IBinder iBinder = new LocalBinder();

    CameraPollingThread cameraPollingThread;
    FilePollingThread filePollingThread;
    ProcessingThread processingThread;
    Looper cameraPollingThreadLooper, processingThreadLooper;

    private Intent intent;

    ArrayBlockingQueue<ToExchange> buffer;

    @Override
    public void onCreate() {
    }

    //using a service to make ATIS polling activity-independent
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "Camera service starting...");

        Intent notificationIntent = new Intent(this, MainActivity.class);
        PendingIntent pendingIntent =
                PendingIntent.getActivity(this, 0, notificationIntent, 0);

        //sticky notification to be able to run service in foreground
        Notification notification =
                new Notification.Builder(this)
                        .setContentTitle("ATIS camera connected")
                        .setContentText("reading events...")
                        .setSmallIcon(R.mipmap.camera_dt)
                        .setContentIntent(pendingIntent)
                        .setTicker("Camera is being polled...")
                        .build();

        startForeground(182903, notification);

        this.intent = intent;
        buffer = new ArrayBlockingQueue<>(2000, true);

        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        stopCameraThreads();
        super.onDestroy();
        Toast.makeText(this, "Camera service has stopped", Toast.LENGTH_SHORT).show();
        Log.d(TAG, "camera service has stopped");
    }

    private void startProducer() {
        Log.d(TAG, "Starting producer thread from service.");
        UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        cameraPollingThread = new CameraPollingThread(intent, usbManager, buffer);
        cameraPollingThread.start();
        cameraPollingThreadLooper = cameraPollingThread.getLooper();
    }

    private void startConsumer() {
        Log.d(TAG, "Starting consumer thread from service.");
        processingThread = new ProcessingThread(buffer);
        processingThread.start();
        processingThreadLooper = processingThread.getLooper();
    }

    public boolean setCameraPolling(boolean flag) {
        if (flag && !isCameraPollingThreadRunning()) {
            startProducer();
            Log.d(TAG, "CameraPollingThread started");
            startConsumer();
            Log.d(TAG, "Consumer started");
            sendBroadcast(new Intent(MainActivity.ACTION_CAMERA_ACTIVATED));
            return true;
        } else if (flag && isCameraPollingThreadRunning()) {
            Log.d(TAG, "CameraPollingThread already running");
            return false;
        } else if (!flag && isCameraPollingThreadRunning()) {
            stopCameraThreads();
            Log.d(TAG, "CameraPollingThread and consumer stopped");
            return true;
        } else if (!flag && !isCameraPollingThreadRunning()) {
            Log.d(TAG, "CameraPollingThread cannot be stopped because it is not running");
            return false;
        } else return false;
    }

    public void startPlaybackThread() {
        startConsumer();

        Kryo kryo = new Kryo();
        kryo.register(ToExchange.class);
        kryo.register(byte[].class);

        try {
            Input input = new Input(new FileInputStream(ProcessingThread.filePath));
            long objectCounter = 0;
            while (!input.end()){
                ToExchange object1 = kryo.readObject(input, ToExchange.class);
                objectCounter++;
            }
            Log.i(TAG, "input ended, counted " + objectCounter + " objects.");
            input.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }

    public boolean isCameraPollingThreadRunning() {
        if (cameraPollingThread == null) {
            return false;
        }
        return cameraPollingThread.isAlive();
    }

    private void stopCameraThreads() {
        if (isCameraPollingThreadRunning()) {
            cameraPollingThread.setCameraAttached(false);
            cameraPollingThreadLooper.quit();
        }
        processingThread.setCameraAttached(false);
        //processingThreadLooper.quitSafely();
    }

    class LocalBinder extends Binder {
        CameraService getService() {
            return CameraService.this;
        }
    }

    //TODO implement bindings for activities
    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return iBinder;
    }

}
