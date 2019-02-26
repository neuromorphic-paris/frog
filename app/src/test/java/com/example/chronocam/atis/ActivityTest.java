package com.example.chronocam.atis;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;

import com.paris.neuromorphic.CameraService;
import com.paris.neuromorphic.MainActivity;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.junit.MockitoJUnit;
import org.mockito.junit.MockitoRule;
import org.robolectric.Robolectric;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.shadows.ShadowToast;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertNotNull;
import static junit.framework.Assert.assertTrue;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.robolectric.Shadows.shadowOf;


/**
 * Created by gregorlenz on 06/12/17.
 */

@RunWith(RobolectricTestRunner.class)
public class ActivityTest {
    @Rule
    public MockitoRule rule = MockitoJUnit.rule();

    @Mock
    private UsbManager usbManager = Mockito.mock(UsbManager.class);
    @Mock
    private UsbDevice usbDevice = Mockito.mock(UsbDevice.class);

    @InjectMocks
    private MainActivity mainActivity = Robolectric.setupActivity(MainActivity.class);

    private Context context;

    @Before
    public void setup() {
        context = mainActivity.getApplicationContext();
    }

    @Test
    public void mainActivity_shouldNotBeNull() {
        assertNotNull(mainActivity);
    }

    @Test
    public void usbBroadCastReceiver_permittedUsbDeviceAttached_shouldStartCameraService() {
        Intent startIntent = new Intent(MainActivity.ACTION_USB_ATTACHED);
        startIntent.putExtra(UsbManager.EXTRA_DEVICE, usbDevice);
        Intent goalIntent = new Intent(context, CameraService.class);

        when(usbManager.hasPermission(usbDevice)).thenReturn(true);
        assertTrue(mainActivity.usbManager.hasPermission(usbDevice));

        mainActivity.usbBroadcastReceiver.onReceive(context, startIntent);
        assertEquals(context.getString(R.string.prepare_camera), ShadowToast.getTextOfLatestToast());

        Intent serviceIntent = shadowOf(mainActivity).getNextStartedService();

        assertTrue(goalIntent.filterEquals(serviceIntent));
        assertEquals(serviceIntent.getComponent(), goalIntent.getComponent());
    }

    @Test
    public void usbBroadCastReceiver_nonPermittedUsbDeviceAttached_shouldRequestPermission() {
        Intent attachIntent = new Intent(MainActivity.ACTION_USB_ATTACHED);
        attachIntent.putExtra(UsbManager.EXTRA_DEVICE, usbDevice);

        when(usbManager.hasPermission(usbDevice)).thenReturn(false);
        assertFalse(mainActivity.usbManager.hasPermission(usbDevice));

        mainActivity.usbBroadcastReceiver.onReceive(context, attachIntent);

        verify(usbManager, times(1)).requestPermission(usbDevice, PendingIntent.getBroadcast(context, 0, new Intent(MainActivity.ACTION_USB_PERMISSION), 0));
    }

    @Test
    public void usbBroadCastReceiver_permissionRequestIsAcceptedByUser_shouldStartCameraService() {
        Intent permissionRequestIntent = new Intent(MainActivity.ACTION_USB_PERMISSION);
        permissionRequestIntent.putExtra(UsbManager.EXTRA_PERMISSION_GRANTED, true);
        Intent goalIntent = new Intent(context, CameraService.class);

        mainActivity.usbBroadcastReceiver.onReceive(context, permissionRequestIntent);
        assertEquals(context.getString(R.string.prepare_camera), ShadowToast.getTextOfLatestToast());

        Intent serviceIntent = shadowOf(mainActivity).getNextStartedService();

        assertTrue(goalIntent.filterEquals(serviceIntent));
        assertEquals(serviceIntent.getComponent(), goalIntent.getComponent());
    }

    @Test
    public void usbBroadCastReceiver_permissionRequestIsDeniedByUser_shouldNotStartService() {
        Intent permissionRequestIntent = new Intent(MainActivity.ACTION_USB_PERMISSION);
        permissionRequestIntent.putExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false);

        mainActivity.usbBroadcastReceiver.onReceive(context, permissionRequestIntent);
        assertEquals(context.getString(R.string.request_grant_camera_permission), ShadowToast.getTextOfLatestToast());
    }

    @Test
    public void usbBroadCastReceiver_usbDeviceDetached_shouldStopService() {
        Intent detachIntent = new Intent(MainActivity.ACTION_USB_DETACHED);

        mainActivity.usbBroadcastReceiver.onReceive(context, detachIntent);

        //TODO assertion
        //assertEquals();
    }

    @Test
    public void startCameraService_shouldStartCorrectServiceClass() {
        mainActivity.startCameraService(false);
        assertEquals(context.getString(R.string.prepare_camera), ShadowToast.getTextOfLatestToast());
        assertEquals(CameraService.class.getName(), shadowOf(mainActivity).getNextStartedService().getComponent().getClassName());
    }
}
