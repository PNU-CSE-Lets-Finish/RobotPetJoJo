package com.cse.LetsFinish.robotpetjojo;

import  android.app.Activity;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.speech.RecognitionListener;
import android.speech.RecognizerIntent;
import android.speech.SpeechRecognizer;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import java.io.IOException;
import java.util.ArrayList;

public class SpeechToTextTask {

    private static final String TAG = "BluetoothClient";

    private Intent i;
    private SpeechRecognizer mRecognizer;
    private Button btn_speech;
    private String text = "JoJo";

    private BluetoothSocket bluetoothSocket;
    private DecodeMessage mDecodeMessage;

    public SpeechToTextTask(Activity activity, BluetoothSocket bluetoothSocket) {
        //bluetooth
        Log.d(TAG, "speechtotexttask");
        this.bluetoothSocket = bluetoothSocket;
        mDecodeMessage = new DecodeMessage();

        i = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
        i.putExtra(RecognizerIntent.EXTRA_CALLING_PACKAGE, activity.getPackageName());
        i.putExtra(RecognizerIntent.EXTRA_LANGUAGE, "ko-KR");

        mRecognizer = SpeechRecognizer.createSpeechRecognizer(activity.getBaseContext());
        mRecognizer.setRecognitionListener(listener);

        btn_speech = (Button) activity.findViewById(R.id.speech_btn);


        // 버튼을 누르면 음성인식 시작
        btn_speech.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                mRecognizer.startListening(i);
            }
        });
    }

    private void write(byte[] data) {
        try{
            bluetoothSocket.getOutputStream().write(data);
        }
        catch(IOException e){e.printStackTrace();}
    }

    public String getText() {
        return text;
    }

    private RecognitionListener listener = new RecognitionListener() {
        @Override
        public void onReadyForSpeech(Bundle params) { }

        @Override
        public void onBeginningOfSpeech() { }

        // 입력받는 소리의 크기
        @Override
        public void onRmsChanged(float rmsdB) { }

        @Override
        public void onBufferReceived(byte[] buffer) { }

        @Override
        public void onEndOfSpeech() { }

        @Override
        public void onError(int error) { }

        // 음성인식 결과
        @Override
        public void onResults(Bundle results) {
            Log.d(TAG, "Speech onResult");
            String sendString = "";
            String key = "";
            key = SpeechRecognizer.RESULTS_RECOGNITION;
            ArrayList<String> mResult = results.getStringArrayList(key);
            String[] rs = new String[mResult.size()];
            mResult.toArray(rs);
            text = ""+rs[0];
            sendString = mDecodeMessage.decodeMessage(text);
            Log.d(TAG, "string to send : " + sendString);
            write(sendString.getBytes());
            btn_speech.setText(text);
        }

        @Override
        public void onPartialResults(Bundle partialResults) { }

        @Override
        public void onEvent(int eventType, Bundle params) { }
    };
}
