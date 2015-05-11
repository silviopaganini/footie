package com.footie.footie;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import java.io.IOException;
import java.util.ArrayList;
import java.util.UUID;


public class MainActivity extends Activity implements View.OnClickListener {

    private Button bScan;
    private TextView tText;
    private ListView lList;

    private ListAdapter mAdapter;

    private BluetoothAdapter mBluetoothAdapter;
    private BroadcastBT mReceiver;
    private BroadcastBT.HandlerComplete mHandler;
    private ArrayList<BluetoothDevice> mDevices;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        bScan = (Button) findViewById(R.id.button);
        tText = (TextView) findViewById(R.id.textView);
        lList = (ListView) findViewById(R.id.listView);

        bScan.setOnClickListener(this);
        lList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                connectToDevice(mDevices.get(i));
            }
        });

        mAdapter = new ListAdapter(this);
        lList.setAdapter(mAdapter);

        mHandler = new BroadcastBT.HandlerComplete() {
            @Override
            public void onComplete(ArrayList<BluetoothDevice> devices) {
                    mDevices = devices;
                    showDevices();
            }
        };
    }

    private void connectToDevice(BluetoothDevice bluetoothDevice) {
        Intent i = new Intent(this,DeviceActivity.class);
        i.putExtra("device",bluetoothDevice);
        startActivity(i);
    }

    private void showDevices() {
        mAdapter.setArray(mDevices);
        mAdapter.notifyDataSetChanged();
    }


    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.button:
                if (checkBTActive()) {
                    tText.setText(R.string.start_Scan);
                    scanBT();
                } else
                    tText.setText(R.string.bluetooth_not_active);

        }
    }

    private boolean checkBTActive() {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) return false;
        if (!mBluetoothAdapter.isEnabled()) return false;
        //Intent enableBluetooth = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        //startActivityForResult(enableBluetooth, 0);
        return true;
    }

    private void scanBT() {
        IntentFilter filter = new IntentFilter();

        mReceiver = new BroadcastBT();
        mReceiver.setHandler(mHandler);

        filter.addAction(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        registerReceiver(mReceiver,filter);
        mBluetoothAdapter.startDiscovery();


    }
}
