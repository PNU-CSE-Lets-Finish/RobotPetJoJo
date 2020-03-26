/* 바퀴 핀번호 할당 */
#define LEFT_WHEEL_MINUS    4
#define LEFT_WHEEL_PLUS     5
#define RIGHT_WHEEL_MINUS   6
#define RIGHT_WHEEL_PLUS    7 


/* 주행 명령어 구분*/
#define FORWARD             1
#define BACKWARD            2
#define TURN_LEFT           3
#define TURN_RIGHT          4



void setup() {

  Serial.begin(9600);
  
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

}

void InitWheel()
{
      digitalWrite(LEFT_WHEEL_MINUS, LOW);
      digitalWrite(LEFT_WHEEL_PLUS, LOW);
      digitalWrite(RIGHT_WHEEL_MINUS,LOW);
      digitalWrite(RIGHT_WHEEL_PLUS,LOW);
}

void loop() {
  
  InitWheel();
  
  if( Serial.available() )
  {
    
    int sel = Serial.parseInt();
    switch( sel )
    {
        // 전진 명령
        case FORWARD:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, HIGH);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,HIGH);
           delay(3000);
           break;

        // 후진 명령
        case BACKWARD:
           digitalWrite(LEFT_WHEEL_MINUS, HIGH);
           digitalWrite(LEFT_WHEEL_PLUS, LOW);
           digitalWrite(RIGHT_WHEEL_MINUS,HIGH);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           delay(3000);
           break;

        // 좌회전 명령
        case TURN_LEFT:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, HIGH);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           delay(3000);
           break;
          
        // 우회전 명령 
        case TURN_RIGHT:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, LOW);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,HIGH);
           delay(3000);
           break;                 
    }    
  }
}
