package com.footie.footie;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import org.w3c.dom.Text;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

/**
 * Created by christian on 11/05/15.
 */
public class DeviceActivity extends Activity implements View.OnClickListener {

    private Button bDump;
    private Button bStop;
    private Button bNew;
    private TextView tText;

    private BluetoothDevice mDevice;
    private OutputStream mmOutputStream;
    private InputStream mmInputStream;
    private BluetoothSocket mmSocket;
    private Thread mThread;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device);


        bDump = (Button) findViewById(R.id.button2);
        bStop = (Button) findViewById(R.id.button3);
        bNew = (Button) findViewById(R.id.button4);
        tText = (TextView) findViewById(R.id.textView3);

        bDump.setOnClickListener(this);

        BluetoothDevice device = getIntent().getExtras().getParcelable("device");
        if (device != null) {
            connect(device);
        }
    }

    private void connect(BluetoothDevice device) {

        UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); //Standard SerialPortService ID
        try {
            mmSocket = device.createRfcommSocketToServiceRecord(uuid);
            mmSocket.connect();
            mmOutputStream = mmSocket.getOutputStream();
            mmInputStream = mmSocket.getInputStream();
            mDevice = device;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onClick(View view) {

        switch (view.getId()) {
            case R.id.button2:
                sendDump();
                break;
            case R.id.button3:
                try {
                    mmOutputStream.write(("stop\n").getBytes());
                } catch (IOException e) {
                    e.printStackTrace();
                }
                break;
            case R.id.button4:

                try {
                    mmOutputStream.write(("new\n").getBytes());
                } catch (IOException e) {
                    e.printStackTrace();
                }
                break;
        }

    }

    private void sendDump() {

        try {
            mmOutputStream.write(("send\n").getBytes());
            if ( mThread != null) {
                mThread.interrupt();
                mThread = null;
            }
            mThread = new Thread(new Runnable() {
                byte delimiter = 10; //This is the ASCII code for a newline character

                @Override
                public void run() {

                    int readBufferPosition = 0;
                    byte[] readBuffer = new byte[1024];

                    while(!Thread.currentThread().isInterrupted()) {
                        try {
                            int bytesAvailable = mmInputStream.available();
                            if(bytesAvailable > 0)  {
                                byte[] packetBytes = new byte[bytesAvailable];
                                mmInputStream.read(packetBytes);
                                for(int i=0;i<bytesAvailable;i++) {
                                    byte b = packetBytes[i];
                                    if(b == delimiter) {
                                        byte[] encodedBytes = new byte[readBufferPosition];
                                        System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                        final String data = new String(encodedBytes, "US-ASCII");
                                        readBufferPosition = 0;
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                tText.setText(tText.getText()+"\n"+data);
                                            }
                                        });
                                    }else
                                        readBuffer[readBufferPosition++] = b;
                                }
                            }
                            bytesAvailable = mmInputStream.available();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }

                }
            });
            mThread.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


}
