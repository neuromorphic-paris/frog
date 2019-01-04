package com.example.chronocam.atis;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.nfc.Tag;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

import java.nio.ByteBuffer;

public class CameraPreview extends View {
    private final String TAG = CameraPreview.class.getName();
    private Bitmap bitmap;
    private ByteBuffer bitmapBuffer;
    public Eventprocessor eventprocessor;

    public CameraPreview(Context context, AttributeSet attributeSet){
        super(context, attributeSet);
        this.bitmap = Bitmap.createBitmap(304, 240, Bitmap.Config.ALPHA_8);
        this.eventprocessor = new Eventprocessor();
        this.bitmapBuffer = eventprocessor.setBitmap(bitmap);
        Log.d(TAG, "test");
    }

    @Override
    protected void onDraw(Canvas canvas){
    /*
        for (int i = 0; i < 304; i++){
            bitmap.setPixel(i, 100, Color.TRANSPARENT);
        }
    */
        int test = eventprocessor.get_JVM_version();
        eventprocessor.renderPreview(bitmapBuffer);
        canvas.drawColor(Color.GRAY);
        canvas.drawBitmap(bitmap, 0, 0, null);
        invalidate();
    }
}
