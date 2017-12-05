package com.example.chronocam.atis;

import android.content.Context;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.util.Log;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.runners.MockitoJUnitRunner;

import java.util.HashMap;

import static org.mockito.Mockito.when;

/**
 * Created by gregorlenz on 05/12/17.
 */

@RunWith(MockitoJUnitRunner.class)
public class UsbUnitTest {
    @Mock
    UsbManager usbManager;
    @Mock
    UsbDevice usbDevice = Mockito.mock(UsbDevice.class);

    @InjectMocks
    MainActivity mainActivity = new MainActivity();

    HashMap<String, UsbDevice> deviceList = new HashMap<>();

    @Before
    public void setup() {
        //mainActivity = new MainActivity();
        Mockito.mock(Log.class);
        deviceList.put("testDevice", usbDevice);
    }

    @Test
    public void testTest() {
        assert mainActivity.checkNumber(3);
    }

    @Test
    public void testGetUsbDevice() {
        when(usbManager.getDeviceList()).thenReturn(deviceList);
        UsbDevice result = mainActivity.getUsbDevice();
        assert result.equals(usbDevice);
    }

    @Test
    public void testUsbDeviceHasPermissions() {
        when(usbManager.hasPermission(usbDevice)).thenReturn(true);
        boolean result = mainActivity.checkPermissions(usbDevice);
        assert result;
    }

    @Test
    public void testUsbDeviceDoesNotHavePermission() {
        when(usbManager.hasPermission(usbDevice)).thenReturn(true);
        boolean result = mainActivity.checkPermissions(usbDevice);
        assert !result;
    }
}
