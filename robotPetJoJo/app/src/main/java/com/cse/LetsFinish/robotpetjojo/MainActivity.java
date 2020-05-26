package com.cse.LetsFinish.robotpetjojo;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Set;

public class MainActivity extends AppCompatActivity {

    private final int REQUEST_BLUETOOTH_ENABLE = 100;
    private final String TAG = "MainActivity";

    private BluetoothAdapter bluetoothAdapter;
    private ArrayList<BluetoothDevice> listItems = new ArrayList<>();
    private ArrayAdapter<BluetoothDevice> listAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ListView listView = findViewById(R.id.list_item);

        if(getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH))
        {bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();}

        if(bluetoothAdapter == null){
            showErrorDialog("This device is not implement Bluetooth.");
            return;
        }

        if(!bluetoothAdapter.isEnabled()){
            Intent mIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(mIntent, REQUEST_BLUETOOTH_ENABLE);
        } else{
            Log.d(TAG, "Initialisation successful.");
            //show devices
        }



        Set<BluetoothDevice> devices = bluetoothAdapter.getBondedDevices();
        final BluetoothDevice[] pairedDevices = devices.toArray(new BluetoothDevice[0]);

        if ( pairedDevices.length == 0 ){
            Toast.makeText(getApplicationContext(), "장치를 찾을 수 없습니다", Toast.LENGTH_LONG).show();
        }

        Collections.addAll(listItems, pairedDevices);
//        for (int i=0;i<pairedDevices.length;i++) {
//            listItems.add(pairedDevices[i]);
//        }

        final BTListAdapter btListAdapter = new BTListAdapter(this, listItems);
        listView.setAdapter(btListAdapter);
        listView.setOnItemClickListener((parent, view, position, id) -> {
            BluetoothDevice device = listItems.get(position);
//                Bundle args = new Bundle();
//                args.putString("device", device.getAddress());
            Toast.makeText(getApplicationContext(), device.toString(), Toast.LENGTH_LONG).show();
            Intent intent = new Intent(getApplicationContext(), CommunicationActivity.class);
            intent.putExtra("device", device);
            startActivity(intent);
            finish();
        });



    }


    public void showErrorDialog(String message)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Quit");
        builder.setCancelable(false);
        builder.setMessage(message);
        builder.setPositiveButton("OK",  new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
//                if ( isConnectionError  ) {
//                    isConnectionError = false;
//                    finish();
//                }
            }
        });
        builder.create().show();
    }





}