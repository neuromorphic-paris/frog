package com.example.chronocam.atis;

public class Eventprocessor {
    private transient long objPtr;

    public Eventprocessor() {
        if (this.objPtr != 0) {
            this.objPtr = new_Eventprocessor();
        }
    }

    protected static long getCPtr(Eventprocessor obj) {
        return (obj == null) ? 0 : obj.objPtr;
    }

    public synchronized void delete() {
        if (objPtr != 0) {
            delete_Eventprocessor(objPtr);
            objPtr = 0;
        }
    }

    public void set_camera_data(byte[] arg0, long arg1) {
        set_camera_data_Eventprocessor(objPtr, this, arg0, arg1);
    }

    private native long new_Eventprocessor();
    private native long delete_Eventprocessor(long jniCPtr);
    private native void set_camera_data_Eventprocessor(long jniCPtr, Eventprocessor eventprocessor, byte[] arg0, long arg1);

}
