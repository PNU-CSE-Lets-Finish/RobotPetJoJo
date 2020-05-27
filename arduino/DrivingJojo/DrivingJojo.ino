#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include <DFPlayer_Mini_Mp3.h>

// 블루투스 핀번호 할당
#define pin_RX              2
#define pin_TX              3

// 초음파 센서 핀번호 할당
#define pin_ECHO            4
#define pin_TRIG            5

// 바퀴 핀번호 할당
#define LEFT_WHEEL_MINUS    7
#define LEFT_WHEEL_PLUS     8
#define RIGHT_WHEEL_MINUS   12
#define RIGHT_WHEEL_PLUS    13

// 양귀, 꼬리 핀번호 할당
#define PIN_EAR_TAIL       6

// 정면 얼굴 회전용 핀번호 할당
#define PIN_LEFT_RIGHT      9
#define PIN_UP_DOWN         10

// 손 핀번호 할당
#define PIN_HAND            11



// 주행 명령어 구분
#define STOP                '0'          // 정지 상태 설정
#define FORWARD             '1'          // 전진 상태 설정
#define BACKWARD            '2'          // 후진 상태 설정
#define TURN_LEFT           '3'          // 좌회전 상태 설정
#define TURN_RIGHT          '4'          // 우회전 상태 설정
#define LEFT_BACK           '5'          // 좌측바퀴 반대회전
#define RIGHT_BACK          '6'          // 우측바퀴 반대회전

// 유저 명령어 구분
#define USR_CALL                'A'      // 스마트폰 마이크에 “죠죠”를 부르면 로봇 펫이 짖는 것으로 응답한다.
#define USR_FOLLOW              'B'      // 스마트폰 마이크에 “이리와” 명령 시 사용자와 가까워지는 방향으로 로봇 펫이 이동한다.
#define USR_BACK                'C'      // 스마트폰 마이크에 “저리가” 명령 시 사용자와 멀어지는 방향으로 로봇 펫이 이동한다.
#define USR_HOLD                'D'      // 스마트폰 마이크에 “멈춰” 명령 시 로봇 펫이 현재 행동을 멈추고 가만히 서있는다.
#define USR_WALK                'E'      // 스마트폰 마이크에 “산책가자” 명령 시 로봇 펫이 귀와 꼬리를 2초간 흔든다
#define USR_TURN                'F'      // 스마트폰 마이크에 “돌아” 명령 시 로봇 펫이 제자리에서 1바퀴 시계방향으로 회전한다.
#define USR_HANDPUSH            'G'      // 스마트폰 마이크에 “손” 명령 시 로봇 펫이 왼 앞발을 사용자 방향으로 내민다.
#define USR_CAREFULLY           'H'      // 스마트폰 마이크에 “쫑긋” 명령 시 로봇 펫이 왼쪽 귀와 오른 쪽 귀를 각각 1초씩 2번 접었다 편다.
#define USR_FORWARD             'I'      // 스마트폰 마이크에 "앞으로 가" 명령 시 로봇펫이 2초간 앞으로 간다.
#define USR_BACKWARD            'J'      // 스마트폰 마이크에 "뒤로 가" 명령 시 로봇펫이 2초간 뒤로 간다.

#define TEST                    'T'      // 전압테스트
#define INIT_ALL                'Z'      // 

  
SoftwareSerial bt(pin_RX,pin_TX); // 블루투스 시리얼 오브젝트
Servo servo_ear_tail;             // 양귀, 꼬리용 서보 오브젝트
Servo servo_hand;                 // 손용 서보 오브젝트
Servo servo_left_right;           // 좌우 얼굴 회전용 서보 오브젝트
Servo servo_up_down;              // 상하 얼굴 회전용 서보 오브젝트

int pos = 0;




Adafruit_AMG88xx amg;
Adafruit_AMG88xx amg2;
float pixels[AMG88xx_PIXEL_ARRAY_SIZE];
float pixels2[AMG88xx_PIXEL_ARRAY_SIZE];


void setup() {

  bt.begin(9600);
  amg.begin(0x68);
  amg2.begin(0x69);
  
  // 초음파 센서 핀모드 설정 및 초기화
  pinMode(pin_ECHO, INPUT);
  pinMode(pin_TRIG, OUTPUT);
  

  // 모터 핀모드 설정 및 초기화
  pinMode(LEFT_WHEEL_MINUS, OUTPUT);
  pinMode(LEFT_WHEEL_PLUS, OUTPUT);
  pinMode(RIGHT_WHEEL_MINUS, OUTPUT);
  pinMode(RIGHT_WHEEL_PLUS, OUTPUT);  
  InitWheel();

  // 각 서보모터 attach 시키기
  servo_ear_tail.attach(PIN_EAR_TAIL);
  servo_hand.attach(PIN_HAND);
  servo_left_right.attach(PIN_LEFT_RIGHT);
  servo_up_down.attach(PIN_UP_DOWN);

  // 얼굴 정면 세팅
  InitServo();


  // DFPlayer 세팅
  Serial.begin(9600);
  mp3_set_serial(Serial);
  delay(1);
  mp3_set_volume(30);
  
  delay(100); // let sensor boot up


}


void InitWheel()
{
      digitalWrite(LEFT_WHEEL_MINUS, LOW);
      digitalWrite(LEFT_WHEEL_PLUS, LOW);
      digitalWrite(RIGHT_WHEEL_MINUS,LOW);
      digitalWrite(RIGHT_WHEEL_PLUS,LOW);
}

void InitServo()
{
  servo_left_right.write(90);
  servo_up_down.write(0);
  servo_hand.write(0);
  servo_ear_tail.write(0);
}

void loop() {


  if( bt.available() )
  {    
    int sel = bt.read();
    int sel2 = bt.read();
    
    Serial.println(sel);
    
    switch( sel )
    {
        case FORWARD:          // 수동조작 전진 명령
           Serial.println("전진명령");
           WheelSetup(sel);
           break;


        case BACKWARD:         // 수동조작 후진 명령
           WheelSetup(sel);
           break;


        case TURN_LEFT:        // 수동조작 좌회전 명령
           WheelSetup(sel);
           break;
          

        case TURN_RIGHT:       // 수동조작 우회전 명령 
           WheelSetup(sel);
           break; 
         
        case STOP:             // 수동조작 정지 명령 
           WheelSetup(sel);
           break;

        case USR_CALL:        // 음성 명령 "죠죠"
           Call();
           break;
           
        case USR_FOLLOW:      // 음성 명령 "이리와"
           Follow();
           break;
           
        case USR_BACK:        // 음성 명령 "저리가"
           Back();
           break;
           
        case USR_HOLD:        // 음성 명령 "멈춰"
           Stop();
           break;
           
        case USR_WALK:        // 음성 명령 "산책가자"
           Walk();
           break;
           
        case USR_TURN:        // 음성 명령 "돌아"
           Turn();
           break;
           
        case USR_HANDPUSH:    // 음성 명령 "손"
           HandPush();
           break;
           
        case USR_CAREFULLY:   // 음성 명령 "쫑긋"
           Craefully();
           break;

        case USR_FORWARD:     // 음성 명령 "앞으로 가"
             Forward();
             break;

        case USR_BACKWARD:    // 음성 명령 "뒤로 가"
            Backward();
            break;           
           
        case TEST:   // 테스트함수
          Test();
          break;

        default :
            WheelSetup(STOP);
    }    
  }
  
}

void WheelSetup( int mode )
{
  switch( mode )
  {
        // 전진 명령
        case FORWARD:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, HIGH);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,HIGH);
           break;

        // 후진 명령
        case BACKWARD:
           digitalWrite(LEFT_WHEEL_MINUS, HIGH);
           digitalWrite(LEFT_WHEEL_PLUS, LOW);
           digitalWrite(RIGHT_WHEEL_MINUS,HIGH);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           break;

        // 좌회전 명령
        case TURN_LEFT:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, LOW);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,HIGH);
           break;
          
        // 우회전 명령 
        case TURN_RIGHT:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, HIGH);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           break; 
        case LEFT_BACK:
           digitalWrite(LEFT_WHEEL_MINUS, HIGH);
           digitalWrite(LEFT_WHEEL_PLUS, LOW);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           break;
          
        case RIGHT_BACK:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, LOW);
           digitalWrite(RIGHT_WHEEL_MINUS,HIGH);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           break; 
        case STOP:
        case USR_HOLD:          
           InitWheel();
           break;   
        case INIT_ALL:
          InitAll();
          break;                
   }
}
void Bark() {
  mp3_next ();
}
void Call(){         // mp3 재생.
  Bark();
}


void Follow(){       // 적외선 열화상 센서, 초음파센서 이용하여 온도총합이 높은 위치를 찾은후 거리를 측정하고 해당 방향으로 전환하여 이동한다.
  
  int temp;
  int temp2;
  int Tmin = 1000, Tmax = 0;
  int Tmin2 = 1000, Tmax2 = 0;
  int int_temp;
  int int_temp2;
  int h,v;
  int total[5][3] = { 0, };  // 2차원 배열의 요소를 모두 0으로 초기화
  int max_temp = 0;
  int max_h, max_v;
  int pos;

  long duration, distance;    // 초음파 센서용 변수

   servo_left_right.write(90);
   servo_up_down.write(0);

  for( pos=90; pos>=45; pos-=15 ){
    servo_left_right.write(pos);
    servo_up_down.write(90-pos);
    delay(300);
  }

  for(v=0; v<1; v++)
  {
    for(h=0; h<5; h++)
    {/*
      if(h!=0)
      {
        for( pos=45+(h-1)*22.5; pos<=45+h*22.5; pos+=4.5 )
          servo_left_right.write(pos);
          delay(500);          
      }*/

      if(h!=0)
        servo_left_right.write(45+h*22.5-11.25);
      delay(100);
      servo_left_right.write(45+h*22.5);
      servo_up_down.write(45+v*22.5);        //정면 서보 회전
      
      amg.readPixels(pixels); // 픽셀 배열 읽어들인다.
      amg2.readPixels(pixels2); // 픽셀 배열 읽어들인다.

      delay(500);       // 지연시간을 설정해 충분히 센서값을 읽을 수 있도록함.  

      for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++) {   // 최소 최대값 구한다.
        temp = pixels[i - 1] * 10;
        Tmin = Tmin > temp ? temp : Tmin;
        Tmax = Tmax < temp ? temp : Tmax;
        
        temp2 = pixels2[i - 1] * 10;
        Tmin2 = Tmin2 > temp2 ? temp2 : Tmin2;
        Tmax2 = Tmax2 < temp2 ? temp2 : Tmax2;
      }
      
      for (int i = AMG88xx_PIXEL_ARRAY_SIZE; i >= 1; i--) {   // 범위를 설정해 값을 맵핑시킨다.
        temp = pixels[i - 1] * 10;
        temp2 = pixels2[i - 1] * 10;
        int_temp = map(temp, Tmin, Tmax, 0, 21); // 온도의 범위를 21단계로 나눔
        int_temp2 = map(temp2, Tmin2, Tmax2, 0, 21); // 온도의 범위를 21단계로 나눔
        
        if( int_temp>=12 || int_temp2>=12 )
          total[h][v]++;      // 기준치보다 높다면 온도 총합에 포함          
      }        
      
    }
  }


  


  for(v=0; v<1; v++)        // 온도 총합이 최대인 위치를 찾음.
  {
    for(h=0; h<5; h++)
    {
      if( max_temp < total[h][v] )
      {
        max_temp = total[h][v];
        max_h = h;
        max_v = v;
      }
    }
  }

    for( pos=135; pos>=45+max_h*22.5; pos-=11.25)
    {
      servo_left_right.write(pos);
      delay(50);
    }
      //온도 총합이 최대인 위치로 정면을 바라봄
      servo_left_right.write(45+max_h*22.5);
      servo_up_down.write(45+max_v*22.5);   

      delay(500); // 초음파 센서 준비시간
          
      digitalWrite(pin_TRIG, LOW);
      delayMicroseconds(2);
      digitalWrite(pin_TRIG, HIGH);
      delayMicroseconds(10);
      digitalWrite(pin_TRIG, LOW);
      
      duration = pulseIn (pin_ECHO, HIGH); //물체에 반사되어돌아온 초음파의 시간을 변수에 저장합니다.
      //34000*초음파가 물체로 부터 반사되어 돌아오는시간 /1000000 / 2(왕복값이아니라 편도값이기때문에 나누기2를 해줍니다.)
     //초음파센서의 거리값이 위 계산값과 동일하게 Cm로 환산되는 계산공식 입니다. 수식이 간단해지도록 적용했습니다.
      distance = duration * 17 / 1000;
      
      switch( max_h )
      {
        case 0:
          WheelSetup(TURN_LEFT);
          delay(1000);
          WheelSetup(STOP);
          break;
        case 1:
          WheelSetup(TURN_LEFT);
          delay(600);
          WheelSetup(STOP);
          break;
        case 2:
          break;
        case 3:
          WheelSetup(TURN_RIGHT);
          delay(600);
          WheelSetup(STOP);
          break;
        case 4:
          WheelSetup(TURN_RIGHT);
          delay(1000);
          WheelSetup(STOP);
          break;
      }
      delay(500);
      WheelSetup(FORWARD);
      delay(20*distance); // 1m 당 2초 전진
      WheelSetup(STOP);

      // 다시 정면을 바라봄
      servo_left_right.write(90);
      servo_up_down.write(0);

      

      
}


void Back(){         // 적외선 열화상 센서, 초음파센서 이용하여 온도총합이 높은 위치를 찾은후 거리를 측정하고 해당방향과 반대로 멀어진다.
    
  int temp;
  int temp2;
  int Tmin = 1000, Tmax = 0;
  int Tmin2 = 1000, Tmax2 = 0;
  int int_temp;
  int int_temp2;
  int h,v;
  int total[5][3] = { 0, };  // 2차원 배열의 요소를 모두 0으로 초기화
  int max_temp = 0;
  int max_h, max_v;
  int pos;

  long duration, distance;    // 초음파 센서용 변수

   servo_left_right.write(90);
   servo_up_down.write(0);

  for( pos=90; pos>=45; pos-=15 ){
    servo_left_right.write(pos);
    servo_up_down.write(90-pos);
    delay(300);
  }

  for(v=0; v<1; v++)
  {
    for(h=0; h<5; h++)
    {/*
      if(h!=0)
      {
        for( pos=45+(h-1)*22.5; pos<=45+h*22.5; pos+=4.5 )
          servo_left_right.write(pos);
          delay(500);          
      }*/

      if(h!=0)
        servo_left_right.write(45+h*22.5-11.25);
      delay(100);
      servo_left_right.write(45+h*22.5);
      servo_up_down.write(45+v*22.5);        //정면 서보 회전
      
      amg.readPixels(pixels); // 픽셀 배열 읽어들인다.
      amg2.readPixels(pixels2); // 픽셀 배열 읽어들인다.

      delay(500);       // 지연시간을 설정해 충분히 센서값을 읽을 수 있도록함.  

      for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++) {   // 최소 최대값 구한다.
        temp = pixels[i - 1] * 10;
        Tmin = Tmin > temp ? temp : Tmin;
        Tmax = Tmax < temp ? temp : Tmax;
        
        temp2 = pixels2[i - 1] * 10;
        Tmin2 = Tmin2 > temp2 ? temp2 : Tmin2;
        Tmax2 = Tmax2 < temp2 ? temp2 : Tmax2;
      }
      
      for (int i = AMG88xx_PIXEL_ARRAY_SIZE; i >= 1; i--) {   // 범위를 설정해 값을 맵핑시킨다.
        temp = pixels[i - 1] * 10;
        temp2 = pixels2[i - 1] * 10;
        int_temp = map(temp, Tmin, Tmax, 0, 21); // 온도의 범위를 21단계로 나눔
        int_temp2 = map(temp2, Tmin2, Tmax2, 0, 21); // 온도의 범위를 21단계로 나눔
        
        if( int_temp>=12 || int_temp2>=12 )
          total[h][v]++;      // 기준치보다 높다면 온도 총합에 포함          
      }        
      
    }
  }


  


  for(v=0; v<1; v++)        // 온도 총합이 최대인 위치를 찾음.
  {
    for(h=0; h<5; h++)
    {
      if( max_temp < total[h][v] )
      {
        max_temp = total[h][v];
        max_h = h;
        max_v = v;
      }
    }
  }

    for( pos=135; pos>=45+max_h*22.5; pos-=11.25)
    {
      servo_left_right.write(pos);
      delay(50);
    }
      //온도 총합이 최대인 위치로 정면을 바라봄
      servo_left_right.write(45+max_h*22.5);
      servo_up_down.write(45+max_v*22.5);   

      delay(500); // 초음파 센서 준비시간
          
      digitalWrite(pin_TRIG, LOW);
      delayMicroseconds(2);
      digitalWrite(pin_TRIG, HIGH);
      delayMicroseconds(10);
      digitalWrite(pin_TRIG, LOW);
      
      duration = pulseIn (pin_ECHO, HIGH); //물체에 반사되어돌아온 초음파의 시간을 변수에 저장합니다.
      //34000*초음파가 물체로 부터 반사되어 돌아오는시간 /1000000 / 2(왕복값이아니라 편도값이기때문에 나누기2를 해줍니다.)
     //초음파센서의 거리값이 위 계산값과 동일하게 Cm로 환산되는 계산공식 입니다. 수식이 간단해지도록 적용했습니다.
      distance = duration * 17 / 1000;
      
      switch( max_h )
      {
        case 0:
          WheelSetup(LEFT_BACK);
          delay(1000);
          WheelSetup(STOP);
          break;
        case 1:
          WheelSetup(LEFT_BACK);
          delay(600);
          WheelSetup(STOP);
          break;
        case 2:
          break;
        case 3:
          WheelSetup(RIGHT_BACK);
          delay(600);
          WheelSetup(STOP);
          break;
        case 4:
          WheelSetup(RIGHT_BACK);
          delay(1000);
          WheelSetup(STOP);
          break;
      }
      delay(500);
      WheelSetup(BACKWARD);
      delay(20*distance); // 1m 당 2초 후진
      WheelSetup(STOP);

      // 다시 정면을 바라봄
      servo_left_right.write(90);
      servo_up_down.write(0);

      
}

void Walk(){         // 귀, 꼬리 2초간 흔들기
  for( int i=0; i<1; i++)
  {
    for( pos=0; pos<=100; pos+=10) 
    {
      servo_ear_tail.write(pos);
      delay(50);
    }
    for( pos=100; pos>=0; pos-=10)
    {
      servo_ear_tail.write(pos);
      delay(50);
    }
  }
}

void Turn(){        // 시계방향 회전
  WheelSetup(TURN_RIGHT);
  delay(3000);
  WheelSetup(STOP);
}

void HandPush(){   // 왼손 내밀기
  
  for( pos=0; pos<=100; pos+=10) 
  {
    servo_hand.write(pos);
    delay(50);
  }
  
  delay(3000);
  
  for( pos=100; pos>=0; pos-=10)
  {
    servo_hand.write(pos);
    delay(50);
  }
}


void Craefully(){   // 귀를 2번 접었다 편다.
  for( int i=0; i<2; i++)
  {
    for( pos=0; pos<=100; pos+=10) 
    {
      servo_ear_tail.write(pos);
      delay(50);
    }
    for( pos=100; pos>=0; pos-=10)
    {
      servo_ear_tail.write(pos);
      delay(50);
    }
  }
}

void Forward(){   // 2초간 전진
  WheelSetup(FORWARD);
  delay(2000);
  WheelSetup(STOP);
}

void Backward(){  // 2초간 후진
  WheelSetup(BACKWARD);
  delay(2000);
  WheelSetup(STOP);
  
}


void InitAll(){   // 서보모터 및 바퀴 동작 정지
  InitWheel();
  InitServo();
}

void Stop(){      // 동작 중지 함수
  InitAll();
}

void Test(){      // 전압 테스트용
  float vout = 0.0;   
  float vin = 0.0;  
  float R1 = 30000.0;  
  float R2 = 7500.0;  
  int value = 0;
  
  value = analogRead(A0);
  vout = (value * 5.0) / 1024.0;  //전압값을 계산해주는 공식입니다.
  vin = vout / ( R2 / ( R1 + R2) );

  Serial.print("V: ");
  Serial.println(vin); //현재1.5V 4채널 건전지의 전압값을 출력해줍니다.

  delay(1000);
}
