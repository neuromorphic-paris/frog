package com.example.chronocam.atis;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.HashMap;
import java.util.Iterator;

public class MainActivity extends AppCompatActivity {
    static {
         //System.loadLibrary("atis_java"); // load libatis_java.so
         System.loadLibrary("eventprocessor");
    }
    private static final String TAG = MainActivity.class.getName();
    static final String ACTION_USB_PERMISSION = "com.example.chronocam.atis.MainActivity.USB_PERMISSION";
    static final String ACTION_USB_ATTACHED = "android.hardware.usb.action.USB_DEVICE_ATTACHED";
    static final String ACTION_USB_DETACHED = "android.hardware.usb.action.USB_DEVICE_DETACHED";

    UsbManager usbManager;
    BroadcastReceiver usbBroadcastReceiver;
    Intent cameraServiceIntent;

    Button startButton;
    TextView infoText;
    ImageView cameraImage;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        startButton = findViewById(R.id.start_recording_button);
        infoText = findViewById(R.id.text_info);
        cameraImage = findViewById(R.id.image_status);

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
                                    Toast.makeText(getApplicationContext(), R.string.request_grant_camera_permission, Toast.LENGTH_SHORT).show();
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

    public native String stringFromJNI();

    @Override
    public void onStop() {
        super.onStop();
        unregisterReceiver(usbBroadcastReceiver);
    }

    void startCameraService() {
        Toast.makeText(getApplicationContext(), R.string.prepare_camera, Toast.LENGTH_SHORT).show();
        cameraServiceIntent = new Intent(this, CameraService.class);
        //cameraServiceIntent.putExtra("usbDevice", getUsbDevice());
        //cameraServiceIntent.putExtra("filePaths", filePaths);
        startService(cameraServiceIntent);
        startButton.setEnabled(true);
        infoText.setText(getString(R.string.press_start_to_record));
    }

    void stopCameraService() {

    }

    UsbDevice getUsbDevice() {
        HashMap<String, UsbDevice> devices = usbManager.getDeviceList();
        if (!devices.isEmpty()) {
            Iterator<String> it = devices.keySet().iterator();
            String deviceName = it.next();
            return devices.get(deviceName);
        } else return null;
    }

    boolean checkPermissions(UsbDevice device) {
        if (device != null && usbManager != null && usbManager.hasPermission(device)) {
            Log.i("checkPermissions", "USB device attached, already has permissions");
            return true;
        } else if (device == null || usbManager == null) {
            Log.w("checkPermissions", "Tried to check permissions although no device present!");
            return false;
        } else {
            Log.d("checkPermissions", "USB device attached, does not have permissions");
            PendingIntent mPermissionIntent = PendingIntent.getBroadcast(getApplicationContext(), 0, new Intent(ACTION_USB_PERMISSION), 0);
            usbManager.requestPermission(device, mPermissionIntent);
            return false;
        }
    }
}
