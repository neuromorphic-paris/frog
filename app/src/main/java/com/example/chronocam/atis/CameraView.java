package com.example.chronocam.atis;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

class CameraView extends View {
    private Bitmap bitmap;
    private static native void setBitmap(Bitmap bitmap);

    public CameraView(Context context, AttributeSet attributeSet) {
        super(context, attributeSet);
        int[] attrsArray = new int[] {
                android.R.attr.layout_width, // 0
                android.R.attr.layout_height // 1
        };
        TypedArray ta = context.obtainStyledAttributes(attributeSet, attrsArray);
        int layout_width = ta. getLayoutDimension(0, ViewGroup.LayoutParams.MATCH_PARENT);
        int layout_height = ta. getLayoutDimension(1, ViewGroup.LayoutParams.MATCH_PARENT);
        ta.recycle();
        bitmap = Bitmap.createBitmap(layout_width, layout_height, Bitmap.Config.RGB_565);
        setBitmap(bitmap);
    }

    @Override protected void onDraw(Canvas canvas) {
        canvas.drawBitmap(bitmap, 0, 0, null);
        invalidate();
    }
}
