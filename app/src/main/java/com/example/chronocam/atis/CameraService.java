package com.example.chronocam.atis;

import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.support.annotation.Nullable;
import android.util.Log;
import android.widget.Toast;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

/**
 * Created by gregorlenz on 05/12/17.
 */

public class CameraService extends Service {
    private static final String TAG = CameraService.class.getName();
    //private final IBinder iBinder = new LocalBinder();

    CameraPollingThread cameraPollingThread;
    Looper cameraPollingThreadLooper;
    ProcessingThread processingThread;

    private Intent intent;

    Handler resultHandler;

    BlockingQueue<ChangeDetection> buffer;

    @Override
    public void onCreate() {
    }

    //using a service to make ATIS polling activity-independent
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Toast.makeText(this, "Camera is starting...", Toast.LENGTH_SHORT).show();
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
        buffer = new ArrayBlockingQueue<>(50000);

        startProducer();
        startConsumer();

        Toast.makeText(this, "Camera has been started successfully!", Toast.LENGTH_SHORT).show();

        return START_REDELIVER_INTENT;
    }

    void startProducer() {
    }

    void startConsumer() {
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    public class LocalBinder extends Binder {
        CameraService getService() {
            return CameraService.this;
        }
    }
}
