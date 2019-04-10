package com.example.chronocam.atis;

public class IS_Usb {
    static {
        System.loadLibrary("atis_java"); // load libatis_java.so
    }
    private static IV_USB iv_usb_;

    public static void set_usb(IV_USB iv_usb) {
        iv_usb_ = iv_usb;
    }

    public final static native void set_JNIEnv();

    public final static native void load_firmware(String firmware);

    public final static void callfromc(int jarg1) {
        ncalled_++;
        teststring_ = new String("I was  called from c " + Integer.toString(ncalled_));
        System.out.println("ICI Java");
    }

    private static String teststring_ = new String("not called from c yet");
    private static int ncalled_ = 0;

    public static String getStringCallback() {
        return teststring_;
    }


    public static int control_read_16bits(int command, int address) {
        return iv_usb_.control_read_16bits(command, address);
    }

    public static int control_read_32bits(int command, int address) {
        return iv_usb_.control_read_32bits(command, address);
    }

    public static void control_transfer_write(int command, int address, int val) {
        System.out.println("Java IS_Usb control_transfer_write");
        iv_usb_.control_tranfer_write(command, address, val);

    }

    public static void control_transfer_write_vector(int command, int val[]) {
        iv_usb_.control_transfer_write_vector(command, val);

    }

    public static void control_transfer_write_data(int command, int address, int index, byte val[]) {
        iv_usb_.control_transfer_write_data(command, address, index, val);
    }
}


