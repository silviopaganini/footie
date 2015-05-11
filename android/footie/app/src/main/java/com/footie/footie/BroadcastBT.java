package com.footie.footie;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import java.util.ArrayList;

/**
 * Created by christian on 11/05/15.
 */
public class BroadcastBT extends BroadcastReceiver {
    private ArrayList<BluetoothDevice> devices;

    private HandlerComplete mHandler;

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
            //discovery starts, we can show progress dialog or perform other tasks
            devices = new ArrayList<>();
        } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
            //discovery finishes, dismis progress dialog
            if (mHandler != null) {
                mHandler.onComplete(devices);
            }
        } else if (BluetoothDevice.ACTION_FOUND.equals(action)) {
            //bluetooth device found
            BluetoothDevice device = (BluetoothDevice) intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            devices.add(device);
        }
    }

    public void setHandler(HandlerComplete mHandler) {
        this.mHandler = mHandler;
    }

    public interface  HandlerComplete {
         void onComplete(ArrayList<BluetoothDevice> devices);
    }
}
