package com.example.chronocam.atis;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.HashMap;
import java.util.Iterator;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = MainActivity.class.getName();
    //static {
    //     System.loadLibrary("atis"); // load libatis_java.so
    //}
    static final String ACTION_USB_PERMISSION = "com.example.chronocam.atis.MainActivity.USB_PERMISSION";
    static final String ACTION_USB_ATTACHED = "android.hardware.usb.action.USB_DEVICE_ATTACHED";
    static final String ACTION_USB_DETACHED = "android.hardware.usb.action.USB_DEVICE_DETACHED";

    UsbManager usbManager;
    BroadcastReceiver usbBroadcastReceiver;
    Intent cameraServiceIntent;
    Eventprocessor eventprocessor;

    //CAMERA preview
    private CameraPreviewTimer cameraPreviewTimer;//Handles periodically camera preview updates
    private Handler previewReceiver;//Handle timer response and updates the view

    //Camera Service
    CameraService cameraService;
    BroadcastReceiver gestureResultReceiver;

    Button startButton;
    TextView infoText;
    ImageView cameraImage;
    CameraPreview cameraPreview;

    String filePath;

    boolean isServiceBound = false;
    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            CameraService.LocalBinder binder = (CameraService.LocalBinder) service;
            cameraService = binder.getService();
            isServiceBound = true;
        }
        @Override
        public void onServiceDisconnected(ComponentName name) {
            isServiceBound = false;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (ACTION_USB_ATTACHED.equalsIgnoreCase(getIntent().getAction())) {
            Log.d("onCreate", "created activity from intent");
        }

        infoText = findViewById(R.id.text_info);
        cameraImage = findViewById(R.id.image_status);

        filePath = Util.copyResource(getApplicationContext(), "dvs.es");
        Log.d(TAG, filePath);
        cameraPreview = findViewById(R.id.camera_preview);
        //eventprocessor = cameraPreview.eventprocessor;

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
                                startCameraService(true);
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
                                    startCameraService(true);
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
        UsbDevice usbDevice = getUsbDevice();
        if (usbDevice != null && checkPermissions(usbDevice)) {
            startCameraService(false);
        }else{
            Log.i(TAG, "no USB device found.");
        }
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

    public void startCameraService(boolean delay) {
        cameraImage.setImageResource(R.mipmap.camera_ok);
        new AsyncCameraStart().execute(delay);
        Toast.makeText(getApplicationContext(), "Preparing camera, please standby", Toast.LENGTH_SHORT).show();
        cameraPreview.setBackgroundColor(Color.WHITE);
    }

    public void stopCameraService(){
        //if (cameraPreviewTimer != null) {
        //    cameraPreviewTimer.cancel();
        //    cameraPreviewTimer = null;
        //}
        if (isServiceBound) {
            unbindService(serviceConnection);
            isServiceBound = false;
        }
        if (cameraServiceIntent != null){
            stopService(cameraServiceIntent);
        }
        cameraImage.setImageResource(R.mipmap.camera_ko);
        //cameraPreview.setImageResource(0);
        //resultTextView.setText("");
        //resultIconView.setImageResource(0);
    }
    //Task to initiate camera
    private class AsyncCameraStart extends AsyncTask<Boolean, Integer, Void> {
        @Override
        protected void onPreExecute() {
        }
        @Override
        protected Void doInBackground(Boolean... params) {
            boolean delay = params[0];
            if (delay) {
                try {
                    Thread.sleep(5000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            cameraServiceIntent = new Intent(getApplicationContext(), CameraService.class);
            cameraServiceIntent.putExtra("usbDevice", getUsbDevice());
            startService(cameraServiceIntent);
            bindService(cameraServiceIntent, serviceConnection, Context.BIND_AUTO_CREATE);
            return null;
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        Log.d("onNewIntent", "new Intent received " + intent.getAction());
        if (ACTION_USB_ATTACHED.equalsIgnoreCase(intent.getAction())) {
        } else if (ACTION_USB_DETACHED.equalsIgnoreCase(intent.getAction())) {

        }
    }

    @Override
    public void onStop() {
        super.onStop();
        unregisterReceiver(usbBroadcastReceiver);
    }

    @Override
    public void onDestroy() {
        stopCameraService();
        super.onDestroy();
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
