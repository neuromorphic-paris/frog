package com.example.chronocam.atis;

import android.os.Handler;
import android.os.Message;

import java.util.Timer;
import java.util.TimerTask;

class CameraPreviewTimer extends Timer {
    private final String TAG = getClass().getName();
    private Handler handler;

    CameraPreviewTimer(Handler handler, long period) {
        // Each pixel is stored as a single translucency (alpha) channel.
        this.handler = handler;
        schedule(new CameraPreviewTask(), 0, period);
    }

    class CameraPreviewTask extends TimerTask {
        private Message message;

        @Override
        public void run() {
            if (ProcessingThread.preview != null) {
                message = Message.obtain();
                message.obj = ProcessingThread.preview;
                if (handler!=null)
                    handler.sendMessage(message);
            }
        }
    }
}