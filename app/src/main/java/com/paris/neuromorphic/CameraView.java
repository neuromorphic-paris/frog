package com.paris.neuromorphic;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

class CameraView extends View {

    public CameraView(Context context, AttributeSet attributeSet) {
        super(context, attributeSet);
        int[] attrsArray = new int[]{
                android.R.attr.layout_width, // 0
                android.R.attr.layout_height // 1
        };
        TypedArray ta = context.obtainStyledAttributes(attributeSet, attrsArray);
        int viewWidth = ta.getLayoutDimension(0, ViewGroup.LayoutParams.MATCH_PARENT);
        int viewHeigth = ta.getLayoutDimension(1, ViewGroup.LayoutParams.MATCH_PARENT);
        ta.recycle();

        Bitmap bitmap = Bitmap.createBitmap(viewWidth, viewHeigth, Bitmap.Config.ALPHA_8);
        Eventprocessor.setSharedBitmap(bitmap);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        Eventprocessor.updateSharedBitmap();
        invalidate();
    }

}
