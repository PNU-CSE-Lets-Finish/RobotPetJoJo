package com.cse.LetsFinish.robotpetjojo;

public class DecodeMessage {
    public DecodeMessage() {
    }

    public String decodeMessage(String codeString){
        switch (codeString) {
            case "죠죠": case "조조": case "지워줘": case "져 줘": case "짖어": case "주적":
                return Constants.BARK;
            case "이리 와": case "따라 와":
                return Constants.COMEHERE;
            case "저리 가": case "가":
                return Constants.GOAWAY;
            case "멈춰":
                return Constants.HOLD;
            case "산책 갈까": case "산책갈까": case "산책 가자":
                return Constants.WALK;
            case "돌아":
                return Constants.TURN;
            case "손": case "손 줘":
                return Constants.HANDPUSH;
            case "쫑긋":
                return Constants.CAREFULLY;
            case "앞으로 가": case "앞으로": case "아프로":
                return Constants.FORWARD;
            case "뒤로": case "뒤로 가":
                return Constants.BACKWARD;
            default:
                return Constants.NONE;
        }
    }

    class Constants {

        // 명령어
        static final String STOP = "0";
        static final String TURNLEFT = "3";
        static final String TURNRIGHT = "4";
        static final String LEFT_BACK = "5";
        static final String RIGHT_BACK = "6";
        //유저 명령어
        static final String BARK = "A";
        static final String COMEHERE = "B";
        static final String GOAWAY = "C";
        static final String HOLD = "D";
        static final String WALK = "E";
        static final String TURN = "F";
        static final String HANDPUSH = "G";
        static final String CAREFULLY = "H";
        static final String FORWARD = "I";
        static final String BACKWARD = "J";


        //아무것도 아닐때
        static final String NONE = "Z";


        private Constants() {}
    }
}
