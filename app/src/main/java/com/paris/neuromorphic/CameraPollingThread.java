package com.paris.neuromorphic;

import android.content.Intent;
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
import com.example.chronocam.atis.IS_Usb;
import com.example.chronocam.atis.USB_Android;

import java.util.concurrent.BlockingQueue;

/**
 * Created by gregorlenz on 26/12/17.
 */

public class CameraPollingThread extends HandlerThread {
    private final String TAG = getClass().getName();
    private Intent intent;
    private UsbManager usbManager;

    //Flags
    private volatile boolean isCameraAttached = true;

    //Counters
    private long eventCount;
    private long maxCount;
    private long iterationCount;
    private long time;

    //static Bitmap preview;
    AtisInstance cam_;
    static byte[] bytes;

    private final BlockingQueue buffer;

    //Timestamps
    private long startTimeStamp;
    private long currentTimeStamp;
    private long lastTimeStamp;

    private static final int PACKET_SIZE = 128 * 1024;
    private static final int TIMEOUT = 100;

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

    CameraPollingThread(Intent intent, UsbManager usbManager, BlockingQueue blockingQueue) {
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

    private void pollCamera() {
        if (intent.hasExtra("usbDevice")) {
            UsbDevice usbDevice = intent.getParcelableExtra("usbDevice");
            String biasFilePath = intent.getStringExtra("filePath");

            UsbInterface intf = usbDevice.getInterface(0);
            UsbDeviceConnection connection = usbManager.openDevice(usbDevice);
            connection.claimInterface(intf, true);
            UsbEndpoint endpoint = null;
            if (intf.getEndpointCount() > 0)
                endpoint = intf.getEndpoint(0);

            final USB_Android usb_android = new USB_Android(connection, usbDevice, endpoint);

            IS_Usb.set_usb(usb_android);
            IS_Usb.set_JNIEnv();

            AtisBiases biases = AtisBiases.from_file(biasFilePath);

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
                        if (size == 16384) {
                            maxCount++;
                        }
                        iterationCount++;
                        EventExchange copyExchange = new EventExchange(toExchange);

                        Log.d(TAG, "Producer: Iteration " + iterationCount + " took " + (currentTimeStamp - lastTimeStamp) + "ns, bulkTransfer took " + (currentTimeStamp - startTimeStamp) + "ns, size: " + size);
                        lastTimeStamp = currentTimeStamp;

                        //put events into buffer
                        buffer.put(copyExchange);
                        toExchange.size = 0;
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    usb_android.release();
                }
            }
            Log.d(TAG, "ended the loop");
        }
    }

    @Override
    public boolean quit() {
        return super.quit();
    }
}
