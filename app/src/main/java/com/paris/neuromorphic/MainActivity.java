package com.paris.neuromorphic;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Objects;

import androidx.appcompat.app.AppCompatActivity;
import butterknife.BindView;
import butterknife.ButterKnife;


public class MainActivity extends AppCompatActivity {
    private final String TAG = getClass().getName();

    static final String ACTION_USB_PERMISSION = "com.paris.neuromorphic.MainActivity.USB_PERMISSION";
    static final String ACTION_USB_ATTACHED = "android.hardware.usb.action.USB_DEVICE_ATTACHED";
    static final String ACTION_USB_DETACHED = "android.hardware.usb.action.USB_DEVICE_DETACHED";
    static final String ACTION_CAMERA_ACTIVATED = "com.paris.neuromorphic.camera.activated";
    final String ASSETS_FILE_BIASES = "standard_new.bias";
    String cameraBiasFilePath, exampleFilePath;

    UsbManager usbManager;
    BroadcastReceiver usbBroadcastReceiver, serviceCallBackReceiver;

    CameraService cameraService;
    Intent cameraServiceIntent;
    boolean isServiceBound = false;

    @BindView(R.id.text_info)
    TextView infoText;
    @BindView(R.id.image_status)
    ImageView cameraStatusImage;
    @BindView(R.id.camera_preview)
    CameraView cameraPreview;
    @BindView(R.id.start_recording_button)
    Button startButton;

    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            cameraService = ((CameraService.LocalBinder) service).getService();
            Log.d(TAG, "Bound CameraService.");
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
        ButterKnife.bind(this);

        if (ACTION_USB_ATTACHED.equalsIgnoreCase(getIntent().getAction())) {
            Log.d(TAG, "created activity from intent");
        }

        exampleFilePath = Util.copyResource(getApplicationContext(), "dvs.es");
        Log.d(TAG, ".es example file path: " + exampleFilePath);
        cameraBiasFilePath = Util.copyResource(getApplicationContext(), ASSETS_FILE_BIASES);

        Eventprocessor.newEventprocessor();

        cameraPreview.setBackgroundColor(Color.GRAY);

        startButton.setEnabled(true);
        startButton.setOnClickListener(view -> {
            startCameraReplacementFilePolling();
        });
    }

    @Override
    public void onStart() {
        super.onStart();
        setUpUSBReceiver();
        setUpServiceCallBackReceiver();
    }

    public void startCameraService() {
        cameraStatusImage.setImageResource(R.mipmap.camera_wait);
        infoText.setText("");
        cameraServiceIntent = new Intent(getApplicationContext(), CameraService.class);
        cameraServiceIntent.putExtra("usbDevice", getUsbDevice());
        cameraServiceIntent.putExtra("filePath", cameraBiasFilePath);
        Eventprocessor.resetBitmap();
        startService(cameraServiceIntent);
        bindService(cameraServiceIntent, serviceConnection, Context.BIND_AUTO_CREATE);
    }

    public void startCameraReplacementFilePolling() {
        startCameraService();
        new AsyncPlaybackStart().execute();
    }

    void stopCameraService() {
        if (isServiceBound) {
            unbindService(serviceConnection);
            isServiceBound = false;
        }
        if (cameraServiceIntent != null) {
            cameraService.setCameraPolling(false);
            stopService(cameraServiceIntent);
        }
        cameraStatusImage.setImageResource(R.mipmap.camera_ko);
        infoText.setText(R.string.no_camera_connected);
        Eventprocessor.resetBitmap();
    }

    public class AsyncPlaybackStart extends AsyncTask<Boolean, Integer, Void> {
        @Override
        protected Void doInBackground(Boolean... params) {
            try {
                Thread.sleep(500);
            } catch (Exception e) {
                e.printStackTrace();
            }
            cameraService.startPlaybackThread();
            return null;
        }
    }

    public class AsyncCameraStart extends AsyncTask<Boolean, Integer, Void> {
        @Override
        protected Void doInBackground(Boolean... params) {
            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            cameraService.setCameraPolling(true);
            return null;
        }
    }

    class AsyncSepia extends AsyncTask<Boolean, Integer, Void> {
        @Override
        protected Void doInBackground(Boolean... booleans) {
            Eventprocessor.resetBitmap();
            Eventprocessor.triggerSepia(exampleFilePath);
            return null;
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        Log.d("onNewIntent", "new Intent received " + intent.getAction());
    }

    @Override
    public void onStop() {
        super.onStop();
        unregisterReceiver(usbBroadcastReceiver);
        unregisterReceiver(serviceCallBackReceiver);
    }

    @Override
    public void onDestroy() {
        stopCameraService();
        super.onDestroy();
        Eventprocessor.deleteBitmap();
    }

    void setUpServiceCallBackReceiver() {
        serviceCallBackReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                switch (Objects.requireNonNull(intent.getAction())) {
                    case ACTION_CAMERA_ACTIVATED:
                        cameraStatusImage.setImageResource(R.mipmap.camera_ok);
                    default:
                        Log.w("BroadcastReceiver", "received unknown Intent");
                }
            }
        };
        registerReceiver(serviceCallBackReceiver, new IntentFilter(ACTION_CAMERA_ACTIVATED));
    }

    void setUpUSBReceiver() {
        //control the CameraService when camera is plugged in/removed
        usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        usbBroadcastReceiver = new BroadcastReceiver() {
            // see device_filter.xml for list of allowed devices
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                final String TAG = "BroadCastReceiver";
                if (action != null) {
                    switch (action) {
                        case ACTION_USB_ATTACHED:
                            Log.i(TAG, "USB device attached");
                            UsbDevice usbDevice = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                            if (checkPermissions(usbDevice)) {
                                startCameraService();
                                new AsyncCameraStart().execute();
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
                                    new AsyncCameraStart().execute();
                                } else {
                                    Log.d(TAG, "Permissions denied by user");
                                    Toast.makeText(getApplicationContext(), R.string.request_grant_camera_permission, Toast.LENGTH_SHORT).show();
                                }
                            }
                            break;
                        default:
                            Log.w(TAG, "received unknown Intent");
                    }
                }
            }
        };

        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_USB_ATTACHED);
        filter.addAction(ACTION_USB_DETACHED);
        filter.addAction(ACTION_USB_PERMISSION);
        registerReceiver(usbBroadcastReceiver, filter);

        UsbDevice usbDevice = getUsbDevice();
        if (usbDevice != null && checkPermissions(usbDevice)) {
            startCameraService();
            new AsyncCameraStart().execute();
        } else {
            Log.v(TAG, "no USB device found.");
        }
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
