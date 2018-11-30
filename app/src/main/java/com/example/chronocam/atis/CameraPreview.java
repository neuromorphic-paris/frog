package com.example.chronocam.atis;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.View;

public class CameraPreview extends View {
    private Bitmap bitmap;
    private int scale = 3;
    private Eventprocessor eventprocessor;

    public CameraPreview(Context context, AttributeSet attributeSet){
        super(context);
        this.bitmap = Bitmap.createBitmap(304, 240, Bitmap.Config.ALPHA_8);
        this.eventprocessor = new Eventprocessor();
    }

    @Override
    protected void onDraw(Canvas canvas){
        for (int i = 40; i < 304; i++){
            bitmap.setPixel(i, 80, Color.WHITE);
            bitmap.setPixel(i, 81, Color.WHITE);
            bitmap.setPixel(i, 82, Color.WHITE);
            bitmap.setPixel(i, 83, Color.TRANSPARENT);
            bitmap.setPixel(i, 84, Color.TRANSPARENT);
            bitmap.setPixel(i, 85, Color.TRANSPARENT);
        }
        eventprocessor.renderPreview(bitmap);
        canvas.drawColor(Color.GRAY);
        canvas.drawBitmap(bitmap, 0, 0, null);
        invalidate();
    }
}
