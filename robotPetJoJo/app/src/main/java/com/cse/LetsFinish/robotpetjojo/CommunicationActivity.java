package com.cse.LetsFinish.robotpetjojo;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.lang.reflect.Method;
import java.util.UUID;

public class CommunicationActivity extends AppCompatActivity {

    //터미널 fragment에서 복사한다.
    //connect, connected 과정을 연구하고 붙여넣는다.
    //말했을 때, 지정한 스트링이 나오면 보냄과 동시에 화면에 보낸다는 것을 표시한다.

    //new bluetooth
    private enum Connected { False, Pending, True }
    static BluetoothAdapter mBluetoothAdapter;
    private Connected connected = Connected.False;
    private BluetoothSocket bluetoothSocket;
    private BluetoothDevice deviceToConnect;

    private static final UUID BLUETOOTH_SPP = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    SpeechToTextTask stt;

    private TextView mConnectionStatus;


    private static final String TAG = "BluetoothClient";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_communication);

        mConnectionStatus = (TextView)findViewById(R.id.tv_bluetooth_status);
        // input text 통과 - 채팅용으로 불필요
        // 리스트 뷰 통과 - 채팅 메세지 출력용으로 불필요

        findConnectedDevice();

        Log.d(TAG, "deviceToConnect : " + deviceToConnect);

        stt = new SpeechToTextTask(this, bluetoothSocket);
    }

    private BluetoothSocket createBluetoothSocket(BluetoothDevice device)throws IOException{
        try{
            final Method m = device.getClass().getMethod("createInsecureRfcommSocketToServiceRecord", UUID.class);
            return (BluetoothSocket) m.invoke(device, BLUETOOTH_SPP);
        } catch(Exception e){ Log.e(TAG, "could not create Insecure RFComm Connection", e);}
        return device.createRfcommSocketToServiceRecord(BLUETOOTH_SPP);
    }

    public void findConnectedDevice(){
        Intent intent = getIntent();
        deviceToConnect = (BluetoothDevice)intent.getExtras().get("device");
        Log.d(TAG, "deviceAddress : " + deviceToConnect);
        try {
            bluetoothSocket = deviceToConnect.createRfcommSocketToServiceRecord(BLUETOOTH_SPP);
            bluetoothSocket.connect();
        } catch (IOException e) {
            e.printStackTrace();
            try {
                bluetoothSocket.close();
            } catch (Exception ignored) {
            }
            bluetoothSocket = null;
            return;
        }
        connected = Connected.True;
        mConnectionStatus.setText("연결되었습니다.");
    }


    private void disconnect(){
        try{
            bluetoothSocket.close();
        }
        catch(IOException e){e.printStackTrace();}
        bluetoothSocket = null;
        connected = Connected.False;
    }


    @Override
    protected void onDestroy() {
        if (connected != Connected.False)
            disconnect();
        super.onDestroy();
    }

    private void receive(byte[] data) {
//        receiveText.append(new String(data));
    }

    public void showQuitDialog(String message)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Quit");
        builder.setCancelable(false);
        builder.setMessage(message);
        builder.setPositiveButton("OK",  new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                finish();
            }
        });
        builder.create().show();
    }

}
