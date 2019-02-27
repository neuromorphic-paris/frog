package com.paris.neuromorphic;

import com.paris.neuromorphic.CameraService;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.Robolectric;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.android.controller.ServiceController;

import static junit.framework.Assert.assertNotNull;

/**
 * Created by gregorlenz on 06/12/17.
 */

@RunWith(RobolectricTestRunner.class)
public class ServiceTest {
    CameraService cameraService;
    ServiceController<CameraService> serviceController;

    @Before
    public void setup() {
        serviceController = Robolectric.buildService(CameraService.class);
        cameraService = serviceController.get();
    }

    @Test
    public void serviceNotNull() {
        assertNotNull(cameraService);
    }
}
