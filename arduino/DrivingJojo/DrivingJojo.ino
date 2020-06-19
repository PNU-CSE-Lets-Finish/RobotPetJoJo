#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include <DFPlayer_Mini_Mp3.h>
#include <MsTimer2.h>

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

#define INIT_ALL                'Z'      // 서보모터 및 바퀴 동작정지 상태

#define H_SIZE                   5        // 열화상센서에서 수평위치 횟수를 결정
#define V_SIZE                   1        // 열화상센서에서 수직위치 횟수를 결정

#define TRUE  1
#define FALSE 0
#define STACK_LEN  100

typedef int Data;

typedef struct _arrayStack        // 스택 구조체 설정
{
  Data stackArr[STACK_LEN];
  int topIndex;
} ArrayStack;

typedef ArrayStack Stack;




  
SoftwareSerial bt(pin_RX,pin_TX); // 블루투스 시리얼 오브젝트
Servo servo_ear_tail;             // 양귀, 꼬리용 서보 오브젝트
Servo servo_hand;                 // 손용 서보 오브젝트
Servo servo_left_right;           // 좌우 얼굴 회전용 서보 오브젝트
Servo servo_up_down;              // 상하 얼굴 회전용 서보 오브젝트

int pos = 0;                      // 임시 위치변수

Adafruit_AMG88xx amg;                     // 좌측 열화상 센서용 오브젝트
Adafruit_AMG88xx amg2;                    // 우측 열화상 센서용 오브젝트
float pixels[AMG88xx_PIXEL_ARRAY_SIZE];   // 좌측 열화상 센서의 데이터를 담기 위한 픽셀 배열
float pixels2[AMG88xx_PIXEL_ARRAY_SIZE];  // 우측 열화상 센서의 데이터를 담기 위한 픽셀 배열

Stack cmdstk;        // 명령어를 담기 위한 스택
int stop_flag = 0;   // 정지명령 플래그


void setup() {

  bt.begin(9600);         // 블루투스 시리얼 설정
  amg.begin(0x68);        // 좌측 열화상 센서 주소 설정 (optional)
  amg2.begin(0x69);       // 우측 열화상 센서 주소 설정 (default)
  
  MsTimer2::set(500, ISR_Timer);  // 500ms 주기로 Timer ISR 호출
  MsTimer2::start();          // 타이머 인터럽트 활성화
  
  // 초음파 센서 핀모드 설정 및 초기화
  pinMode(pin_ECHO, INPUT);
  pinMode(pin_TRIG, OUTPUT);  

  // 모터 핀모드 설정 및 초기화
  pinMode(LEFT_WHEEL_MINUS, OUTPUT);
  pinMode(LEFT_WHEEL_PLUS, OUTPUT);
  pinMode(RIGHT_WHEEL_MINUS, OUTPUT);
  pinMode(RIGHT_WHEEL_PLUS, OUTPUT);  

  // 각 서보모터 attach 시키기
  servo_ear_tail.attach(PIN_EAR_TAIL);
  servo_hand.attach(PIN_HAND);
  servo_left_right.attach(PIN_LEFT_RIGHT);
  servo_up_down.attach(PIN_UP_DOWN);

  // 정면얼굴, 바퀴, 팔, 귀, 꼬리 서보모터 초기화
  InitAll();

  // 스택 초기화
  StackInit(&cmdstk);
  
  // DFPlayer 세팅
  Serial.begin(9600);
  mp3_set_serial(Serial);
  delay(1);
  mp3_set_volume(30);
  
  delay(100); // 센서가 부팅 될 수 있는 최소 시간 설정

  
  

}

void loop() {
    
  int sel = 0;
  stop_flag = 0;
  
  if( bt.available() )
  {
    SPush( &cmdstk, bt.read() );  // 명령어를 스택에 저장.
    bt.read(); // 음성인식에서 블루투스가 같은값 연속으로 2번 보내는 bug때문에 하나의 값을 버리도록 함.
  }


  if( !SIsEmpty(&cmdstk) )    // 실행할 명령어가 있다면 (수신된 명령어가 스택에 남아있다면)
  {    
    sel = SPop(&cmdstk);
    
    switch( sel )             // sel의 값에 따라 조작을 설정
    {
        case FORWARD:          // 수동조작 전진 명령이 전달되었을 때
           WheelSetup(sel);
           break;


        case BACKWARD:         // 수동조작 후진 명령이 전달되었을 때
           WheelSetup(sel);
           break;


        case TURN_LEFT:        // 수동조작 좌회전 명령이 전달되었을 때
           WheelSetup(sel);
           break;
          

        case TURN_RIGHT:       // 수동조작 우회전 명령이 전달되었을 때
           WheelSetup(sel);
           break; 
         
        case STOP:             // 수동조작 정지 명령이 전달되었을 때
           WheelSetup(sel);
           break;

        case USR_CALL:        // 음성 명령 "죠죠"가 전달되었을 때
           Call();
           break;
           
        case USR_FOLLOW:      // 음성 명령 "이리와"가 전달되었을 때
           Follow();
           break;
           
        case USR_BACK:        // 음성 명령 "저리가"가 전달되었을 때
           Back();
           break;
           
        case USR_HOLD:        // 음성 명령 "멈춰"가 전달되었을 때
           Stop();
           break;
           
        case USR_WALK:        // 음성 명령 "산책가자"가 전달되었을 때
           Walk();
           break;
           
        case USR_TURN:        // 음성 명령 "돌아"가 전달되었을 때
           Turn();
           break;
           
        case USR_HANDPUSH:    // 음성 명령 "손"이 전달되었을 때
           HandPush();
           break;
           
        case USR_CAREFULLY:   // 음성 명령 "쫑긋"이 전달되었을 때
           Craefully();
           break;

        case USR_FORWARD:     // 음성 명령 "앞으로 가"가 전달되었을 때
             Forward();
             break;

        case USR_BACKWARD:    // 음성 명령 "뒤로 가"가 전달되었을 때
            Backward();
            break;           
           
        case INIT_ALL:  // 서보모터 위치 초기화 및 바퀴 초기화
          InitAll();
          break;       

        default :   // 예상외의 명령이 전달되었을 때
          InitAll();
          break;       
    }    
  }
  
}


void InitWheel()      // 바퀴 상태를 정지 상태로 초기화
{
      digitalWrite(LEFT_WHEEL_MINUS, LOW);
      digitalWrite(LEFT_WHEEL_PLUS, LOW);
      digitalWrite(RIGHT_WHEEL_MINUS,LOW);
      digitalWrite(RIGHT_WHEEL_PLUS,LOW);
}

void InitServo()
{
  servo_hand.write(0);          // 양손 서보모터를 0도 위치로 초기화
  servo_ear_tail.write(180);      // 양귀, 꼬리 서보모터를 0도 위치로 초기화
}

void InitHead()
{
   // 정면 얼굴 서보모터 위치 초기화 (좌우 90도 상하 0도)
   servo_left_right.write(90);
   servo_up_down.write(0);
}

void WheelSetup( int mode )     // 설정된 mode 인자에 따라 바퀴의 상태를 변화시키는 함수  
{
  switch( mode )
  {
        // 전진 명령 - 좌,우바퀴 모두 동작
        case FORWARD:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, HIGH);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,HIGH);
           break;

        // 후진 명령 - 좌,우바퀴 모두 반대로 동작
        case BACKWARD:
           digitalWrite(LEFT_WHEEL_MINUS, HIGH);
           digitalWrite(LEFT_WHEEL_PLUS, LOW);
           digitalWrite(RIGHT_WHEEL_MINUS,HIGH);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           break;

        // 좌회전 명령 - 우 바퀴만 동작
        case TURN_LEFT:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, LOW);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,HIGH);
           break;
          
        // 우회전 명령 - 좌 바퀴만 동작
        case TURN_RIGHT:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, HIGH);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           break; 
        
        // 좌측바퀴만 반대로 동작하는 상태
        case LEFT_BACK:
           digitalWrite(LEFT_WHEEL_MINUS, HIGH);
           digitalWrite(LEFT_WHEEL_PLUS, LOW);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           break;

        // 우측바퀴만 반대로 동작하는 상태
        case RIGHT_BACK:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, LOW);
           digitalWrite(RIGHT_WHEEL_MINUS,HIGH);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           break; 

        // STOP 또는 USR_HOLD
        case STOP:
        case USR_HOLD:          
           InitWheel();
           break;           
   }
}

void Follow(){       // 적외선 열화상 센서, 초음파센서 이용하여 온도총합이 높은 위치를 찾은후 거리를 측정하고 해당 방향으로 전환하여 이동한다.
  
  int temp;                       // 좌측 열화상 센서의 임시 온도변수
  int temp2;                      // 우측 열화상 센서의 임시 온도변수
  int Tmin = 1000, Tmax = 0;      // 좌측 열화상 센서의 온도 최소값, 최대값의 초기화
  int Tmin2 = 1000, Tmax2 = 0;    // 우측 열화상 센서의 온도 최소값, 최대값의 초기화
  int int_temp;                   // 21단계로 나뉜 온도의 세기 (좌측 센서)
  int int_temp2;                  // 21단계로 나뉜 온도의 세기 (우측 센서)
  int h,v;                                  // 수평, 수직 이동횟수를 결정하기 위한 변수
  int total[H_SIZE][V_SIZE] = { 0, };       // 각 위치별로 기준치보다 높은 온도 횟수 총합을 체크하기위한 변수. 각 픽셀에서 기준치보다 높은 온도가 있다면 횟수 총합을 증가시킴. 2차원 배열의 요소를 모두 0으로 초기화
  int max_temp = 0;
  int max_h, max_v;                         // 기준치보다 높은 온도 횟수 총합이 제일 큰 위치

  long duration, distance;    // 초음파 센서용 변수

   // 정면 얼굴 서보모터 위치 초기화 (좌우 90도 상하 0도)
   InitHead();

  // 정면 얼굴 서보모터의 위치를 최초 센서값을 읽어들일 위치로 이동하는데 회전하면서 충격을 주지 않기 위해 부드럽게 조금씩 이동하는 모습. (좌우, 상하 서보모터 각각 45도 위치로 최종 이동한다.)
  for( pos=90; pos>=45; pos-=15 ){  
    servo_left_right.write(pos);
    servo_up_down.write(90-pos);
    delay(300);
  }


  for(v=0; v<V_SIZE; v++)      // v는 수직 각도 증가량으로, 수직 이동 횟수를 결정 (현재 값은 수직으로 1번만 이동함.) 
  {
    for(h=0; h<H_SIZE; h++)    // h는 수평 각도 증가량으로, 수평 이동 횟수를 결정 (현재 값은 수평으로 5번 이동함.)
    {
      
      if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;

      if(h!=0)
        servo_left_right.write(45+h*22.5-11.25); // 수평 각도 증가량이 22.5도이지만 바로 22.5도로 움직이지 않고 11.25도 감소된 값으로 움직여 움직임에 충격을 줄임.
      delay(100);
      servo_left_right.write(45+h*22.5);   // 수평 각도 증가량은 22.5도
      servo_up_down.write(45+v*22.5);      // 수직 각도 증가량은 22.5도
      
      amg.readPixels(pixels);   // 좌측 열화상 센서의 픽셀 배열 읽어들여 pixel 배열 변수에 저장한다
      amg2.readPixels(pixels2); // 우측 열화상 센서의 픽셀 배열 읽어들여 pixel2 배열 변수에 저장한다

      delay(500);       // 지연시간을 설정해 충분히 센서값을 읽을 수 있도록함.  

      
      // 온도의 최소 최대값 구한다.
      for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++) {     // 1~64까지 index 값이 바뀜
        temp = pixels[i - 1] * 10;              // index 값 위치의 픽셀 데이터를 읽어들여 온도변수에 저장(좌측 센서)
        Tmin = Tmin > temp ? temp : Tmin;       // 온도 최소값이 계속 갱신된다.(좌측 센서)
        Tmax = Tmax < temp ? temp : Tmax;       // 온도 최대값이 계속 갱신된다.(좌측 센서)
        
        temp2 = pixels2[i - 1] * 10;            // index 값 위치의 픽셀 데이터를 읽어들여 온도변수에 저장(우측 센서)
        Tmin2 = Tmin2 > temp2 ? temp2 : Tmin2;  // 온도 최소값이 계속 갱신된다.(우측 센서)
        Tmax2 = Tmax2 < temp2 ? temp2 : Tmax2;  // 온도 최대값이 계속 갱신된다.(우측 센서)
      }
      
      // 범위를 설정해 값을 맵핑시킨다.
      for (int i = AMG88xx_PIXEL_ARRAY_SIZE; i >= 1; i--) {    // 1~64까지 index 값이 바뀜
        temp = pixels[i - 1] * 10;              // index 값 위치의 픽셀 데이터를 읽어들여 온도변수에 저장(좌측 센서)
        temp2 = pixels2[i - 1] * 10;            // index 값 위치의 픽셀 데이터를 읽어들여 온도변수에 저장(우측 센서)
        int_temp = map(temp, Tmin, Tmax, 0, 21);     // 온도의 범위를 21단계로 나눔(좌측 센서)
        int_temp2 = map(temp2, Tmin2, Tmax2, 0, 21); // 온도의 범위를 21단계로 나눔(우측 센서)
        
        if( int_temp>=12 || int_temp2>=12 )    // 기준치보다 높다면 횟수 총합을 증가시킨다. 현재 setting된 기준치는 12
          total[h][v]++;                       // 해당 h와 v일때의 횟수 총합이 증가함.
      }        
      
    }
  }


  

  // 기준치보다 높은 온도의 총합이 제일 큰 위치를 찾음.
  for(v=0; v<V_SIZE; v++)        
  {
    for(h=0; h<H_SIZE; h++)
    {
      if( max_temp < total[h][v] )
      {
        max_temp = total[h][v];
        max_h = h;    // 기준치보다 높은 온도 횟수 총합이 제일 큰 위치의 h 값
        max_v = v;    // 기준치보다 높은 온도 횟수 총합이 제일 큰 위치의 v 값
      }
    }
  }
    
      // 회전 충격을 줄이기 위해 예정 위치보다 천천히 서보모터를 이동시킴
      for( pos=135; pos>=45+max_h*22.5; pos-=11.25)
      {
        servo_left_right.write(pos);
        delay(50);
      }
      
      //온도 총합이 최대인 위치로 정면을 바라봄
      servo_left_right.write(45+max_h*22.5);
      servo_up_down.write(45+max_v*22.5);   

      
      if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;

      delay(500); // 초음파 센서 준비시간
          
      digitalWrite(pin_TRIG, LOW);
      delayMicroseconds(2);
      digitalWrite(pin_TRIG, HIGH);
      delayMicroseconds(10);
      digitalWrite(pin_TRIG, LOW);
      
      duration = pulseIn (pin_ECHO, HIGH); //물체에 반사되어돌아온 초음파의 시간을 변수에 저장합니다.
      distance = duration * 17 / 1000;     //초음파센서의 거리값이 Cm로 환산.  
     
     
      switch( max_h )                      // 온도가 제일 높았던 위치 중에서 수평 위치에 따라 모터의 회전 방향과 회전 정도를 설정
      {
        case 0:                            // 45도라면 1초간 좌회전
          WheelSetup(TURN_LEFT);
          delay(1000);
          WheelSetup(STOP);
          break;
        case 1:                           // 67.5도라면 0.6초간 좌회전
          WheelSetup(TURN_LEFT);          
          delay(600);
          WheelSetup(STOP);
          break;
        case 2:                           // 90도라면 (정면)
          break;
        case 3:                           // 112.5도라면 0.6초간 우회전
          WheelSetup(TURN_RIGHT);
          delay(600);
          WheelSetup(STOP);
          break;
        case 4:                           // 135도라면 1초간 우회전
          WheelSetup(TURN_RIGHT);
          delay(1000);
          WheelSetup(STOP);
          break;
      }
      delay(500);
      
      if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;
       
      if( distance >= 100 )                // 사용자와의 거리가 1m 이상일 시 1m까지만 인식하도록 함. 
        distance = 100;
              
      WheelSetup(FORWARD);
      delay(20*distance);                 // 사용자와의 거리를 좁혀줌. 1m 당 2초 전진 
      WheelSetup(STOP);

      // 다시 정면을 바라봄
      InitHead();
      
}


void Back(){         // 적외선 열화상 센서, 초음파센서 이용하여 온도총합이 높은 위치를 찾은후 거리를 측정하고 해당방향과 반대로 멀어진다.
    
  int temp;                       // 좌측 열화상 센서의 임시 온도변수
  int temp2;                      // 우측 열화상 센서의 임시 온도변수
  int Tmin = 1000, Tmax = 0;      // 좌측 열화상 센서의 온도 최소값, 최대값의 초기화
  int Tmin2 = 1000, Tmax2 = 0;    // 우측 열화상 센서의 온도 최소값, 최대값의 초기화
  int int_temp;                   // 21단계로 나뉜 온도의 세기 (좌측 센서)
  int int_temp2;                  // 21단계로 나뉜 온도의 세기 (우측 센서)
  int h,v;                                  // 수평, 수직 이동횟수를 결정하기 위한 변수
  int total[H_SIZE][V_SIZE] = { 0, };       // 각 위치별로 기준치보다 높은 온도 횟수 총합을 체크하기위한 변수. 각 픽셀에서 기준치보다 높은 온도가 있다면 횟수 총합을 증가시킴. 2차원 배열의 요소를 모두 0으로 초기화
  int max_temp = 0;                         // 횟수 총합이 최대인 위치를 찾기위한 임시변수
  int max_h, max_v;                         // 기준치보다 높은 온도 횟수 총합이 제일 큰 위치 수평, 수직값

  long duration, distance;    // 초음파 센서용 시간, 거리 변수

   // 정면 얼굴 서보모터 위치 초기화 (좌우 90도 상하 0도) 
   InitHead();

  // 정면 얼굴 서보모터의 위치를 최초 센서값을 읽어들일 위치로 이동하는데 회전하면서 충격을 주지 않기 위해 부드럽게 조금씩 이동하는 모습. (좌우, 상하 서보모터 각각 45도 위치로 최종 이동한다.)
  for( pos=90; pos>=45; pos-=15 ){
    servo_left_right.write(pos);
    servo_up_down.write(90-pos);
    delay(300);
  }

  for(v=0; v<V_SIZE; v++)      // v는 수직 각도 증가량으로, 수직 이동 횟수를 결정 (현재 값은 수직으로 1번만 이동함.) 
  {
    for(h=0; h<H_SIZE; h++)    // h는 수평 각도 증가량으로, 수평 이동 횟수를 결정 (현재 값은 수평으로 5번 이동함.)
    {
      
      if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;

      if(h!=0)
        servo_left_right.write(45+h*22.5-11.25); // 수평 각도 증가량이 22.5도이지만 바로 22.5도로 움직이지 않고 11.25도 감소된 값으로 움직여 움직임에 충격을 줄임.
      delay(100);
      servo_left_right.write(45+h*22.5);   // 수평 각도 증가량은 22.5도
      servo_up_down.write(45+v*22.5);      // 수직 각도 증가량은 22.5도
      
      amg.readPixels(pixels);   // 좌측 열화상 센서의 픽셀 배열 읽어들여 pixel 배열 변수에 저장한다
      amg2.readPixels(pixels2); // 우측 열화상 센서의 픽셀 배열 읽어들여 pixel2 배열 변수에 저장한다

      delay(500);       // 지연시간을 설정해 충분히 센서값을 읽을 수 있도록함.  

      // 온도의 최소 최대값 구한다.
      for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++) {     // 1~64까지 index 값이 바뀜
        temp = pixels[i - 1] * 10;              // index 값 위치의 픽셀 데이터를 읽어들여 온도변수에 저장(좌측 센서)
        Tmin = Tmin > temp ? temp : Tmin;       // 온도 최소값이 계속 갱신된다.(좌측 센서)
        Tmax = Tmax < temp ? temp : Tmax;       // 온도 최대값이 계속 갱신된다.(좌측 센서)
        
        temp2 = pixels2[i - 1] * 10;            // index 값 위치의 픽셀 데이터를 읽어들여 온도변수에 저장(우측 센서)
        Tmin2 = Tmin2 > temp2 ? temp2 : Tmin2;  // 온도 최소값이 계속 갱신된다.(우측 센서)
        Tmax2 = Tmax2 < temp2 ? temp2 : Tmax2;  // 온도 최대값이 계속 갱신된다.(우측 센서)
      }
      
      for (int i = AMG88xx_PIXEL_ARRAY_SIZE; i >= 1; i--) {     // 1~64까지 index 값이 바뀜
        temp = pixels[i - 1] * 10;              // index 값 위치의 픽셀 데이터를 읽어들여 온도변수에 저장(좌측 센서)
        temp2 = pixels2[i - 1] * 10;            // index 값 위치의 픽셀 데이터를 읽어들여 온도변수에 저장(우측 센서)
        int_temp = map(temp, Tmin, Tmax, 0, 21);     // 온도의 범위를 21단계로 나눔(좌측 센서)
        int_temp2 = map(temp2, Tmin2, Tmax2, 0, 21); // 온도의 범위를 21단계로 나눔(우측 센서)
        
        if( int_temp>=12 || int_temp2>=12 )    // 기준치보다 높다면 횟수 총합을 증가시킨다. 현재 setting된 기준치는 12
          total[h][v]++;                       // 해당 h와 v일때의 횟수 총합이 증가함.
      }        
      
    }
  }


  


  // 기준치보다 높은 온도의 총합이 제일 큰 위치를 찾음.
  for(v=0; v<1; v++)
  {
    for(h=0; h<5; h++)
    {
      if( max_temp < total[h][v] )
      {
        max_temp = total[h][v];
        max_h = h;    // 기준치보다 높은 온도 횟수 총합이 제일 큰 위치의 h 값
        max_v = v;    // 기준치보다 높은 온도 횟수 총합이 제일 큰 위치의 v 값
      }
    }
  }

    // 회전 충격을 줄이기 위해 예정 위치보다 천천히 서보모터를 이동시킴
    for( pos=135; pos>=45+max_h*22.5; pos-=11.25)
    {
      servo_left_right.write(pos);
      delay(50);
    }
      //온도 총합이 최대인 위치로 정면을 바라봄
      servo_left_right.write(45+max_h*22.5);
      servo_up_down.write(45+max_v*22.5);   
      
      if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;

      delay(500); // 초음파 센서 준비시간
          
      digitalWrite(pin_TRIG, LOW);
      delayMicroseconds(2);
      digitalWrite(pin_TRIG, HIGH);
      delayMicroseconds(10);
      digitalWrite(pin_TRIG, LOW);
      
      duration = pulseIn (pin_ECHO, HIGH); //물체에 반사되어돌아온 초음파의 시간을 변수에 저장
      distance = duration * 17 / 1000;     //초음파센서의 거리값이 Cm로 환산  
      
      switch( max_h )                      // 온도가 제일 높았던 위치 중에서 수평 위치에 따라 모터의 회전 방향과 회전 정도를 설정
      {
        case 0:                            // 45도라면 1초간 좌바퀴 반대회전
          WheelSetup(LEFT_BACK);
          delay(1000);
          WheelSetup(STOP);
          break;
        case 1:                            // 67.5도라면 0.6초간 좌바퀴 반대회전
          WheelSetup(LEFT_BACK);
          delay(600);
          WheelSetup(STOP);
          break;
        case 2:                            // 90도라면 그대로 (정면)
          break;
        case 3:                            // 112.5도라면 0.6초간 우바퀴 반대회전
          WheelSetup(RIGHT_BACK);
          delay(600);
          WheelSetup(STOP);
          break;
        case 4:                            // 135도라면 1초간 우바퀴 반대회전
          WheelSetup(RIGHT_BACK);
          delay(1000);
          WheelSetup(STOP);
          break;
      }
      delay(500);
      
      if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;

      if( distance >= 100 )                // 사용자와의 거리가 1m 이상일 시 1m까지만 인식하도록 함. 
        distance = 100;
        
      WheelSetup(BACKWARD);
      delay(20*distance);                  // 사용자와의 거리를 늘려 줌. 1m 당 2초 후진
      WheelSetup(STOP);

      // 다시 정면을 바라봄
      InitHead();

      
}


void Call(){         // "죠죠" 명령 시 짖는 소리 함수를 호출
  Bark();
}

void Bark() {
  mp3_next ();       // mp3 재생.
}

void Walk(){         // 귀, 꼬리 2초간 흔들기
  for( int i=0; i<1; i++)
  {
    for( pos=180; pos>=80; pos-=10)
    {
      
      if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;
       
      servo_ear_tail.write(pos);
      delay(50);
    }
    for( pos=80; pos<=180; pos+=10) 
    {
      if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;
       
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
    if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;
       
    servo_hand.write(pos);
    delay(50);
  }
  
  delay(3000);
  
  for( pos=100; pos>=0; pos-=10)
  {
    
    if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;
       
    servo_hand.write(pos);
    delay(50);
  }
}


void Craefully(){   // 귀를 2번 접었다 편다.
  for( int i=0; i<2; i++)
  {
    for( pos=180; pos>=80; pos-=10)
    {
      
      if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;
       
      servo_ear_tail.write(pos);
      delay(50);
    }
    for( pos=80; pos<=180; pos+=10) 
    {
      
      if(stop_flag)   // stop 플래그 활성화 시 함수 종료.
       return;
       
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


void InitAll(){   // 정면얼굴, 귀,꼬리,팔 서보모터 및 바퀴 동작 정지
  InitWheel();
  InitServo();
  InitHead();
}

void Stop(){      // 동작 중지 함수
  InitWheel();
  InitServo();
}

void ISR_Timer(){      // 타이머 ISR 호출. 최초 50ms 간격
  static int timer_cnt = 0;
  
  if( bt.available() )
  {
    SPush( &cmdstk, bt.read() );  // 명령어를 스택에 저장.
    bt.read();                    // 음성인식에서 블루투스가 같은값 연속으로 2번 보내는 bug때문에 하나의 값을 버리도록 함.
  }


  if( !SIsEmpty(&cmdstk) )    // 실행할 명령어가 있다면 (수신된 명령어가 스택에 남아있다면)
  {    
    if( SPeek(&cmdstk)== USR_HOLD ) // 정지 명령어라면
     {
        SPop(&cmdstk);
        stop_flag = 1;  // stop 플래그 활성화
     }
  }

  if( timer_cnt%200==0 ) // 100초마다 전압값 업데이트
  {
    float vout = 0.0;   
    float vin = 0.0;  
    float R1 = 30000.0;  
    float R2 = 7500.0;  
    int value = 0;
    
    value = analogRead(A0);
    vout = (value * 5.0) / 1024.0;  //전압값을 계산해주는 공식
    vin = vout / ( R2 / ( R1 + R2) );
    bt.write(vin); // 현재1.5V 4채널 건전지의 전압값을 블루투스로 송신
  }
  
  if(timer_cnt>=199)
    timer_cnt=0;
  else
    timer_cnt++;
}



void StackInit(Stack * pstack)  // 스택 초기화 함수
{
  pstack->topIndex = -1;
}

int SIsEmpty(Stack * pstack)   // 빈 스택 확인 함수
{
  if(pstack->topIndex == -1)
    return TRUE;
  else
    return FALSE;
}

void SPush(Stack * pstack, Data data) // 스택 Push 함수
{
  pstack->topIndex += 1;
  pstack->stackArr[pstack->topIndex] = data;
}

Data SPop(Stack * pstack)             // 스택 Pop 함수
{
  int rIdx;

  rIdx = pstack->topIndex;
  pstack->topIndex -= 1;

  return pstack->stackArr[rIdx];
}

Data SPeek(Stack * pstack)            // 스택 Peek 함수
{
  return pstack->stackArr[pstack->topIndex];
}
