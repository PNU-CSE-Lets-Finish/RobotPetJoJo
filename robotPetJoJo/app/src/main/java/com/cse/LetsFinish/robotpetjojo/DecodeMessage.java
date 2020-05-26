package com.cse.LetsFinish.robotpetjojo;

public class DecodeMessage {
    public DecodeMessage() {
    }

    public String decodeMessage(String codeString){
        switch (codeString) {
            case "앞으로":
                return Constants.FORWARD;
            case "뒤로":
                return Constants.BACKWARD;
            case "죠죠": case "조조": case "지워줘": case "져줘":
                return Constants.CALLING;
            case "산책 갈까":
                return Constants.GOFORAWALK;
            case "앉아":
                return Constants.SITDOWN;
            default:
                return Constants.NONE;
        }
    }

    class Constants {

        // 명령어
        static final String FORWARD = "0";
        static final String BACKWARD = "1";
        static final String CALLING = "5";
        static final String SITDOWN = "6";
        //...
        static final String GOFORAWALK = "A";

        //아무것도 아닐때
        static final String NONE = "Z";


        private Constants() {}
    }
}
