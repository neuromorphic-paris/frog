package com.example.chronocam.atis;

import android.content.Intent;
import android.graphics.Color;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.os.HandlerThread;
import android.util.Log;

import com.chronocam.libatis.Atis;
import com.chronocam.libatis.AtisBiases;
import com.chronocam.libatis.AtisInstance;

import java.util.concurrent.BlockingQueue;

/**
 * Created by gregorlenz on 26/12/17.
 */

public class CameraPollingThread extends HandlerThread {
    private final String TAG = CameraPollingThread.class.getName();
    private Intent intent;
    private UsbManager usbManager;

    //Flags
    private volatile boolean isCameraAttached = true;
    private volatile boolean isRecording = false;
    private volatile boolean predict = false;

    //Counters
    private long eventCount;
    private long maxCount;
    private long iterationCount;
    private long time;

    //static Bitmap preview;
    static byte[][] bytePreview;
    AtisInstance cam_;
    static byte[] bytes;

    private final BlockingQueue buffer;

    //Timestamps
    private long startTimeStamp;
    private long currentTimeStamp;
    private long lastTimeStamp;

    private static final int PACKET_SIZE = 128 * 1024;
    private static final int TIMEOUT = 100;
    private static final int WIDTH = 304;
    private static final int HEIGHT = 240;

    static class ToExchange {
        int size = 0;
        byte data[] = new byte[PACKET_SIZE];
    }

    class EventExchange {
        int size = 0;
        byte data[] = new byte[128 * 1024];

        EventExchange(ToExchange toExchange) {
            this.size = toExchange.size;
            this.data = toExchange.data;
        }
    }
    public CameraPollingThread(Intent intent, UsbManager usbManager, BlockingQueue blockingQueue) {
        super(CameraPollingThread.class.getName());
        this.intent = intent;
        this.usbManager = usbManager;
        this.buffer = blockingQueue;
    }
    @Override
    protected void onLooperPrepared() {
        Log.d(TAG, "Looper prepared");
        if (intent != null) {
            pollCamera();
        }

    }

    @Override
    public void run() {
        Log.d(TAG, "inside run method. ");
        super.run();
    }

    void setCameraAttached(boolean flag) {
        isCameraAttached = flag;
    }

    void setRecordingStatus(boolean flag) {
        iterationCount = 0;
        maxCount = 0;
        eventCount = 0;
        isRecording = flag;
        time = System.nanoTime();
    }

    private void pollCamera() {
        if (intent.hasExtra("usbDevice")) {
            UsbDevice usbDevice = intent.getParcelableExtra("usbDevice");
            String[] paths;
            paths = intent.getStringArrayExtra("filePaths");
            int p = 0;
            for (String path : paths) {
                paths[p] = path;
                p++;
            }

            UsbInterface intf = usbDevice.getInterface(0);
            UsbDeviceConnection connection = usbManager.openDevice(usbDevice);
            connection.claimInterface(intf, true);
            UsbEndpoint endpoint = null;
            if (intf.getEndpointCount() > 0)
                endpoint = intf.getEndpoint(0);

            final USB_Android usb_android = new USB_Android(connection, usbDevice, endpoint);

            IS_Usb.set_usb(usb_android);
            IS_Usb.set_JNIEnv();

            AtisBiases biases = AtisBiases.from_file(paths[0]);

            Eventprocessor eventprocessor = new Eventprocessor();
            //eventprocessor.init(paths[1], paths[2], paths[3]);

            if (biases != null) {
                Atis atis = new Atis();

                Log.d(TAG, "open cam...");
                Log.d(TAG, IS_Usb.getStringCallback());

                cam_ = atis.open_atis_auto("", "");

                Log.d(TAG, "set biases...");
                cam_.set_biases(biases);

                Log.d(TAG, "set couple...");
                cam_.set_couple(false);

                Log.d(TAG, "start cam...");
                cam_.start();
            }


            ToExchange toExchange = new ToExchange();
            //for (int c = 0; c < 2000; c++) {
            while (isCameraAttached) {
                try {
                    startTimeStamp = System.nanoTime();
                    toExchange.size = usb_android.bulkTransfer(0x81, toExchange.data, toExchange.data.length, TIMEOUT);
                    currentTimeStamp = System.nanoTime();

                    int size = toExchange.size;
                    if (size > 0) {
                        if (isRecording) {
                            if (size == 16384) {
                                maxCount++;
                            }
                            iterationCount++;
                            EventExchange copyExchange = new EventExchange(toExchange);

                            Log.d(TAG, "Producer: Iteration " + iterationCount + " took " + (currentTimeStamp - lastTimeStamp) + "ns, bulkTransfer took " + (currentTimeStamp - startTimeStamp) + "ns, size: " + size);
                            lastTimeStamp = currentTimeStamp;
                            buffer.put(copyExchange);
                        } else {
                            process_data_raw(toExchange);
                        }
                        toExchange.size = 0;
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            Log.d(TAG, "ended the loop");
        }
    }

    private void process_data_raw(ToExchange toExchange) {
        long size = toExchange.size;
        for (int i = 0; i <= size; )
            processGrade(toExchange.data[i++], toExchange.data[i++], toExchange.data[i++], toExchange.data[i++]);
    }

    //TODO decode timestamp here
    private void processGrade(byte a, byte b, byte c, byte d) {
        byte p = (byte) ((d & 0xf0) >> 4);
        if (p == 8) {
            //Log.d("processGrade", Integer.toString((((a & 0xff) | ((b & 0xff) << 8) | ((c & 0xff) << 16) | ((d & 0x0f) << 24)) << 11)));
            return;
        }
        int y = a & 0xff;
        if (y < 0 || y >= HEIGHT)
            return;
        int x = (b & 0xff) + ((c & 0x01) << 8);
        if (x < 0 || x >= WIDTH)
            return;
        y = 239 - y;
        if (p > 0) {
            //ProcessingThread.preview.setPixel(x, y, Color.BLACK);
        } else {
            //ProcessingThread.preview.setPixel(x, y, Color.TRANSPARENT);
        }
        //bytePreview[x][y] = p;

        /*eventCount++;
        if (eventCount % 10000 == 0) {
            Log.d(TAG, "10000 events received");
            eventCount = 0;
        }*/
    }

    @Override
    public boolean quit() {
        return super.quit();
    }
}
