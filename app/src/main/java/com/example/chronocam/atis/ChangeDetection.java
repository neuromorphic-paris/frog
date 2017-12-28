package com.example.chronocam.atis;

/**
 * Created by gregorlenz on 11/11/17.
 */

class ChangeDetection {
    private final int x, y;
    private final long timestamp;
    private final boolean polarity;

    ChangeDetection(int x, int y, long timestamp, boolean polarity) {
        this.x = x;
        this.y = y;
        this.timestamp = timestamp;
        this.polarity = polarity;
    }

    ChangeDetection(int x, int y, boolean polarity) {
        this(x, y, 0, polarity);
    }

    int getX() {
        return this.x;
    }

    int getY() {
        return this.y;
    }

    long getTimestamp() {
        return this.timestamp;
    }

    boolean isPolarity() {
        return this.polarity;
    }

    String parametersToString() {
        return "x: " + x + ", y: " + y + ", ts:" + String.valueOf(timestamp) + ", pol: " + String.valueOf(polarity);
    }
}
