package com.paris.neuromorphic;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

class CameraView extends View {
    private Bitmap bitmap;
    int viewWidth, viewHeigth;

    public CameraView(Context context, AttributeSet attributeSet) {
        super(context, attributeSet);
        int[] attrsArray = new int[]{
                android.R.attr.layout_width, // 0
                android.R.attr.layout_height // 1
        };
        TypedArray ta = context.obtainStyledAttributes(attributeSet, attrsArray);
        viewWidth = ta.getLayoutDimension(0, ViewGroup.LayoutParams.MATCH_PARENT);
        viewHeigth = ta.getLayoutDimension(1, ViewGroup.LayoutParams.MATCH_PARENT);
        ta.recycle();
        bitmap = Bitmap.createBitmap(304, 240, Bitmap.Config.ALPHA_8);

        Eventprocessor.setBitmap(bitmap);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        canvas.drawBitmap(Bitmap.createScaledBitmap(bitmap, viewWidth, viewHeigth, false), 0, 0, null);
        invalidate();
    }

}
