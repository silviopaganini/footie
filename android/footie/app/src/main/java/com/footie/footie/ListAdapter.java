package com.footie.footie;


import android.bluetooth.BluetoothClass;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.ArrayList;

public class ListAdapter  extends BaseAdapter {
    private LayoutInflater mInflater;
    private ArrayList<BluetoothDevice> data = null;

    public ListAdapter(Context context) {
        mInflater = LayoutInflater.from(context);
    }

    @Override
    public int getCount() {
        if (data == null) return 0;
        else return data.size();
    }

    @Override
    public Object getItem(int arg0) {
        if (data == null) return null;
        try {
            return data.get(arg0);
        } catch  (Exception e) {
            return null;
        }
    }

    @Override
    public long getItemId(int arg0) {
        if (data == null) return 0;
        try {
            return arg0;
        } catch  (Exception e) {
            return 0;
        }
    }

    @Override
    public View getView(int arg0, View convertView, ViewGroup parent) {
        convertView = (LinearLayout) mInflater.inflate(R.layout.spinner_item,parent, false);
        BluetoothDevice device = data.get(arg0);
        String text= device.getName()+" "+device.getAddress();

        TextView tName = (TextView) convertView.findViewById(R.id.textView2);
        tName.setText(text);
        return convertView;
    }

    public void setArray(ArrayList<BluetoothDevice> data) {
        this.data = data;

    }


}