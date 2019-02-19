package com.example.chronocam.atis;

import java.util.Timer;
import java.util.TimerTask;

public class TestTimer extends Timer {
    private long startTime;

    public TestTimer(){
        startTime = System.currentTimeMillis();
        schedule(new TestTimerTask(), 0, 20);
    }

    private static native void renderPlasma(long time_ms);

    public class TestTimerTask extends TimerTask{
        @Override
        public void run() {
            renderPlasma(System.currentTimeMillis() - startTime);
        }
    }
}
