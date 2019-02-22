package com.example.chronocam.atis;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

class CameraView extends View {
    private Bitmap bitmap, scaledBitmap;
    private Integer viewWidth, viewHeigth;
    private static native void setBitmap(Bitmap bitmap);
    static native void deleteBitmap();
    static native void resetBitmap();

    public CameraView(Context context, AttributeSet attributeSet) {
        super(context, attributeSet);
        int[] attrsArray = new int[] {
                android.R.attr.layout_width, // 0
                android.R.attr.layout_height // 1
        };
        TypedArray ta = context.obtainStyledAttributes(attributeSet, attrsArray);
        viewWidth = ta. getLayoutDimension(0, ViewGroup.LayoutParams.MATCH_PARENT);
        viewHeigth = ta. getLayoutDimension(1, ViewGroup.LayoutParams.MATCH_PARENT);
        ta.recycle();
        bitmap = Bitmap.createBitmap(320, 240, Bitmap.Config.ALPHA_8);
        setBitmap(bitmap);
    }

    @Override protected void onDraw(Canvas canvas) {
        scaledBitmap = Bitmap.createScaledBitmap(bitmap, viewWidth, viewHeigth, false);
        canvas.drawBitmap(scaledBitmap, 0, 0, null);
        scaledBitmap.recycle();
        invalidate();
    }

}
