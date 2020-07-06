package com.cse.LetsFinish.robotpetjojo;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.ArrayList;

public class BTListAdapter extends BaseAdapter {

    private LayoutInflater mLayoutInflater = null;
    private ArrayList<BluetoothDevice> sample;

    public BTListAdapter(Context mContext, ArrayList<BluetoothDevice> sample) {
        this.mLayoutInflater = LayoutInflater.from(mContext);
        this.sample = sample;
    }

    @Override
    public int getCount() {
        return sample.size();
    }

    @Override
    public Object getItem(int position) {
        return sample.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View view = mLayoutInflater.inflate(R.layout.device_list_item, null);

        TextView bluetToothName = view.findViewById(R.id.tv_bt_name);
        TextView blueToothID = view.findViewById((R.id.tv_bt_id));

        bluetToothName.setText(sample.get(position).getName());
        blueToothID.setText(sample.get(position).getAddress());

        return view;
    }
}