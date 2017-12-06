package com.example.chronocam.atis;

import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.junit.MockitoJUnit;
import org.mockito.junit.MockitoRule;
import org.robolectric.Robolectric;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.HashMap;

import static junit.framework.Assert.*;
import static org.mockito.Mockito.*;

/**
 * Created by gregorlenz on 05/12/17.
 */

@RunWith(RobolectricTestRunner.class)
@Config(constants = BuildConfig.class)
public class UsbUnitTest {
    @Rule
    public MockitoRule rule = MockitoJUnit.rule();
    @Mock
    UsbManager usbManager;
    @Mock
    UsbDevice usbDevice = Mockito.mock(UsbDevice.class);
    @InjectMocks
    MainActivity mainActivity = Robolectric.setupActivity(MainActivity.class);

    HashMap<String, UsbDevice> deviceList = new HashMap<>();

    @Before
    public void setup() {
        deviceList.put("testDevice", usbDevice);
    }

    @Test
    public void getUsbDevice_returnsUsbDevice() {
        when(usbManager.getDeviceList()).thenReturn(deviceList);
        UsbDevice result = mainActivity.getUsbDevice();
        assertEquals(result, usbDevice);
    }

    @Test
    public void checkPermissions_validDevice_returnsTrue() {
        when(usbManager.hasPermission(usbDevice)).thenReturn(true);
        boolean result = mainActivity.checkPermissions(usbDevice);
        assertTrue(result);
    }

    @Test
    public void checkPermissions_invalidDevice_returnsFalse() {
        when(usbManager.hasPermission(usbDevice)).thenReturn(false);
        boolean result = mainActivity.checkPermissions(usbDevice);
        assertFalse(result);
    }

    @Test
    public void checkPermissions_nullDevice_returnsFalse(){
        boolean result = mainActivity.checkPermissions(null);
        assertFalse(result);
    }

    @Test
    public void checkPermissions_nullUsbManager_returnsFalse(){
        usbManager = null;
        boolean result = mainActivity.checkPermissions(usbDevice);
        assertFalse(result);
    }
}
