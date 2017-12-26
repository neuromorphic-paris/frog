package com.example.chronocam.atis;

import junit.framework.Assert;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.Robolectric;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.android.controller.ServiceController;
import org.robolectric.annotation.Config;

import static junit.framework.Assert.*;
import static org.mockito.Mockito.*;

/**
 * Created by gregorlenz on 06/12/17.
 */

@RunWith(RobolectricTestRunner.class)
public class ServiceTest {
    CameraService cameraService;
    ServiceController<CameraService> serviceController;

    @Before
    public void setup(){
        serviceController = Robolectric.buildService(CameraService.class);
        cameraService = serviceController.get();
    }

    @Test
    public void serviceNotNull(){
        assertNotNull(cameraService);
    }
}
