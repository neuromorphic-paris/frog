package com.example.chronocam.atis;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.util.Log;

class USB_Android implements IV_USB {

    private static final String THIS_CLASS = "USBAndroid";

    private UsbDevice usbDevice = null;
    private UsbDeviceConnection connection;
    private UsbEndpoint endpoint;
    private int version;

    USB_Android(UsbDeviceConnection connection, UsbDevice device, UsbEndpoint endpoint) {
        this.endpoint = endpoint;
        this.connection = connection;
        this.usbDevice = device;
        version = control_read_cmd((byte) 0x70, 0x00);
        Log.w("Atis", "Version " + version);
    }

    void release() {
        connection.close();
    }

    public int control_read_16bits(int command, int address) {
        byte data[];
        data = new byte[4];

        int r = connection.controlTransfer((byte) 0xC0, (byte) command, (short) address, (short) 0, data, 4, 0);
        if (r <= 0) {
            Log.d(THIS_CLASS,"Error control read");
            return 0;
        }
        int ret = data[2];
        ret |= data[3] << 8;
        return ret;
    }

    public int control_read_32bits(int command, int address) {
        byte data[];
        data = new byte[8];
        int r = connection.controlTransfer((byte) 0xC0, (byte) command, (short) (address & 0xFFFF), (short) ((address >>> 16) & 0xFFFF), data, 8, 0);
        if (r <= 0) {
            Log.d(THIS_CLASS,"Error control read");
            return 0;
        }
        int ret = data[7];
        ret |= data[6] << 8;
        ret |= data[5] << 8;
        ret |= data[4] << 8;
        return ret;
    }

    public int control_read_cmd(byte cmd, int address) {
        byte data[];
        data = new byte[4];

        int r = connection.controlTransfer((byte) 0xC0, (byte) cmd, (short) address, (short) 0, data, 4, 0);
        if (r <= 0) {
            Log.d(THIS_CLASS,"control_read_cmd ");
            return 0;
        }

        int ret = data[2];
        ret |= data[3] << 8;
        return ret;
    }

    public void control_tranfer_write(int command, int address, int val) {
        if (version == 2) {
            control_tranfer_write32(command, address, val);
        } else {
            control_tranfer_write16(command, address, val);
        }
    }

    public void control_tranfer_write16(int command, int address, int val) {
        byte data[];
        data = new byte[2];
        data[0] = (byte) (val & 0xFF);
        data[1] = (byte) ((val >>> 8) & 0xFF);
        if (connection != null)
            connection.controlTransfer(0x40, (byte) command, address, 0, data, 2, 0);

    }

    public void control_tranfer_write32(int command, int address, int val) {
        byte data[];
        data = new byte[4];
        data[0] = (byte) ((val >>> 24) & 0xFF);
        data[1] = (byte) ((val >>> 16) & 0xFF);
        data[2] = (byte) ((val >>> 8) & 0xFF);
        data[3] = (byte) (val & 0xFF);

        if (connection != null)
            connection.controlTransfer((byte) 0x40, (byte) command, (short) (address & 0xFFFF), (short) ((address >>> 16) & 0xFFFF), data, 4, 0);
    }

    public void control_transfer_write_vector(int base_address, int val[]) {
        if (version == 2) {
            control_transfer_write_vector32(base_address, val);
        } else {
            control_transfer_write_vector16(base_address, val);
        }
    }

    public void control_transfer_write_vector16(int command, int val[]) {
        byte data[];
        data = new byte[2 * val.length];

        for (int i = 0; i < val.length; ++i) {
            data[i * 2 + 0] = (byte) (val[i] & 0xFF);
            data[i * 2 + 1] = (byte) ((val[i] >>> 8) & 0xFF);
        }
        if (connection != null)
            connection.controlTransfer(0x40, command, 0x0, 0, data, data.length, 0);
    }

    public void control_transfer_write_vector32(int command, int val[]) {
        byte data[];
        data = new byte[4 * val.length];
        for (int i = 0; i < val.length; ++i) {
            data[i * 4 + 0] = (byte) ((val[i] >>> 24) & 0xFF);
            data[i * 4 + 1] = (byte) ((val[i] >>> 16) & 0xFF);
            data[i * 4 + 2] = (byte) ((val[i] >>> 8) & 0xFF);
            data[i * 4 + 3] = (byte) ((val[i]) & 0xFF);
        }
        if (connection != null)
            connection.controlTransfer((byte) 0x40, (byte) command, (short) 0x0, (short) 0, data, data.length, 0);
    }

    public void control_transfer_write_data(int command, int address, int index, byte val[]) {
        if (connection != null)
            connection.controlTransfer((byte) 0x40, (byte) command, (short) address, (short) index, val, val.length, 0);
    }

    public int bulkTransfer(int endpoint, byte data[], int max_size, int timeout) {
        if (connection == null) return 0;
        int r = connection.bulkTransfer(this.endpoint, data, max_size, timeout);
        if (r < 0) {
            //conected = false;
            Log.d(THIS_CLASS, "bulkTransfer() FAIL");
            return -1;
        }
        //c/onected = true;
        return r;
    }
}

