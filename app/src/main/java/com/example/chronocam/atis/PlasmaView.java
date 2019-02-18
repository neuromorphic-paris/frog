package com.example.chronocam.atis;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

class PlasmaView extends View {
    private Bitmap mBitmap;
    private long mStartTime;

    // implementend by libplasma.so
    private static native void setBitmap(Bitmap bitmap);
    private static native void renderPlasma(long time_ms);

    public PlasmaView(Context context, AttributeSet attributeSet) {
        super(context);
        mBitmap = Bitmap.createBitmap(604, 480, Bitmap.Config.RGB_565);
        mStartTime = System.currentTimeMillis();
        setBitmap(mBitmap);
    }

    @Override protected void onDraw(Canvas canvas) {
        renderPlasma(System.currentTimeMillis() - mStartTime);
        canvas.drawBitmap(mBitmap, 0, 0, null);
        // force a redraw, with a different time-based pattern.
        invalidate();
    }
}
