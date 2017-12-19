package com.example.chronocam.atis;

import org.junit.Before;
import org.junit.runner.RunWith;
import org.robolectric.Robolectric;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.android.controller.ServiceController;
import org.robolectric.annotation.Config;

/**
 * Created by gregorlenz on 06/12/17.
 */

@RunWith(RobolectricTestRunner.class)
@Config(constants = BuildConfig.class)
public class ServiceTest {
    CameraService cameraService;
    ServiceController<CameraService> serviceController;

    @Before
    public void setup(){
        serviceController = Robolectric.buildService(CameraService.class);
        cameraService = serviceController.get();
    }
}
