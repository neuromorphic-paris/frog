package com.example.chronocam.atis;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

class PlasmaView extends View {
    private Bitmap mBitmap;
    private long mStartTime;

    // implementend by libplasma.so
    private static native void setBitmap(Bitmap bitmap);
    private static native void renderPlasma(long time_ms);

    public PlasmaView(Context context, AttributeSet attributeSet) {
        super(context, attributeSet);
        int[] attrsArray = new int[] {
                android.R.attr.layout_width, // 0
                android.R.attr.layout_height // 1
        };
        TypedArray ta = context.obtainStyledAttributes(attributeSet, attrsArray);
        int layout_width = ta. getLayoutDimension(0, ViewGroup.LayoutParams.MATCH_PARENT);
        int layout_height = ta. getLayoutDimension(1, ViewGroup.LayoutParams.MATCH_PARENT);
        ta.recycle();
        mBitmap = Bitmap.createBitmap(layout_width, layout_height, Bitmap.Config.RGB_565);
        mStartTime = System.currentTimeMillis();
        setBitmap(mBitmap);
    }

    @Override protected void onDraw(Canvas canvas) {
        //renderPlasma(System.currentTimeMillis() - mStartTime);
        canvas.drawBitmap(mBitmap, 0, 0, null);
        // force a redraw, with a different time-based pattern.
        invalidate();
    }
}
