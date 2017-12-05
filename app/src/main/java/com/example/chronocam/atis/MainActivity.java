package com.example.chronocam.atis;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import java.util.HashMap;
import java.util.Iterator;

public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("atis_java"); // load libatis_java.so
        System.loadLibrary("eventprocessor");
    }
    private static final String TAG = MainActivity.class.getName();
    private static final String ACTION_USB_PERMISSION = "com.example.chronocam.atis.MainActivity.USB_PERMISSION";
    private static final String ACTION_USB_ATTACHED = "android.hardware.usb.action.USB_DEVICE_ATTACHED";
    private static final String ACTION_USB_DETACHED = "android.hardware.usb.action.USB_DEVICE_DETACHED";

    UsbManager usbManager;
    BroadcastReceiver usbBroadcastReceiver;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        usbBroadcastReceiver = new BroadcastReceiver() {
            // see device_filter.xml for list of allowed devices
            @Override
            public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            final String TAG = "usbBroadCastReceiver";
            if (action != null) {
                switch (action) {
                    case ACTION_USB_ATTACHED:
                        Log.i(TAG, "USB device attached");
                        UsbDevice usbDevice = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                        if (checkPermissions(usbDevice)) {
                            startCameraService();
                        }
                        break;
                    case ACTION_USB_DETACHED:
                        Log.i(TAG, "USB device detached");
                        stopCameraService();
                        break;
                    case ACTION_USB_PERMISSION:
                        Log.i(TAG, "Permission intent received");
                        synchronized (this) {
                            if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                                Log.d(TAG, "Permissions granted");
                                startCameraService();
                            } else {
                                Log.d(TAG, "Permissions denied by user");
                            }
                        }
                        break;
                    default:
                        Log.w("BroadcastReceiver", "received unknown Intent");
                }
            }
            }
        };
    }

    @Override
    public void onStart() {
        super.onStart();
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_USB_ATTACHED);
        filter.addAction(ACTION_USB_DETACHED);
        filter.addAction(ACTION_USB_PERMISSION);

        registerReceiver(usbBroadcastReceiver, filter);
    }

    @Override
    public void onStop() {
        super.onStop();
        unregisterReceiver(usbBroadcastReceiver);
    }

    void startCameraService(){}
    void stopCameraService(){}

    UsbDevice getUsbDevice() {
        HashMap<String, UsbDevice> devices = usbManager.getDeviceList();
        if (!devices.isEmpty()) {
            Iterator<String> it = devices.keySet().iterator();
            String deviceName = it.next();
            return devices.get(deviceName);
        }
        return null;
    }

    boolean checkPermissions(UsbDevice device) {
        if (device != null && usbManager.hasPermission(device)) {
            Log.i("checkPermissions", "USB device attached, already has permissions");
            return true;
        } else if (device == null) {
            Log.i("checkPermissions", "Tried to check permissions although no device present!");
            return false;
        } else {
            Log.i("checkPermissions", "USB device attached, does not have permissions");
            PendingIntent mPermissionIntent = PendingIntent.getBroadcast(getApplicationContext(), 0, new Intent(ACTION_USB_PERMISSION), 0);
            usbManager.requestPermission(device, mPermissionIntent);
            return false;
        }
    }
}
