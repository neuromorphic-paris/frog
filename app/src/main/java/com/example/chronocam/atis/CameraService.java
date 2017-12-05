package com.example.chronocam.atis;

import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.util.Log;
import android.widget.Toast;

/**
 * Created by gregorlenz on 05/12/17.
 */

public class CameraService extends Service {
    private static final String TAG = CameraService.class.getName();
    private final IBinder iBinder = new LocalBinder();

    public class LocalBinder extends Binder {
        CameraService getService() {
            return CameraService.this;
        }
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

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
                        //.setSmallIcon(R.mipmap.camera_dt)
                        .setContentIntent(pendingIntent)
                        .setTicker("Camera is being polled...")
                        .build();

        startForeground(182903, notification);

        //start thread which is actually retrieving the data from the camera
        startCameraThread();

        return START_REDELIVER_INTENT;
    }

    @Override
    public void onDestroy() {
        stopCameraThread();
        super.onDestroy();
        Toast.makeText(this, "camera service has stopped", Toast.LENGTH_SHORT).show();
        Log.d(TAG, "camera service has stopped");
    }

    void startCameraThread() {
    }

    void stopCameraThread() {
    }
}
