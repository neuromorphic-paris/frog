package com.example.chronocam.atis;

public interface IV_USB {
    public int control_read_16bits(int command, int address);

    public int control_read_32bits(int command, int address);

    public void control_tranfer_write(int command, int address, int val);

    public void control_transfer_write_vector(int command, int val[]);

    public void control_transfer_write_data(int command, int address, int index, byte val[]);

    public int bulkTransfer(int endpoint, byte data[], int max_size, int timeout);
}
