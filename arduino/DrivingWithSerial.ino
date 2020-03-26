
#define LEFT_WHEEL_MINUS    4
#define LEFT_WHEEL_PLUS     5
#define RIGHT_WHEEL_MINUS   6
#define RIGHT_WHEEL_PLUS    7 

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

void InitializeWheel()
{
      digitalWrite(LEFT_WHEEL_MINUS, LOW);
      digitalWrite(LEFT_WHEEL_PLUS, LOW);
      digitalWrite(RIGHT_WHEEL_MINUS,LOW);
      digitalWrite(RIGHT_WHEEL_PLUS,LOW);
}

void loop() {
  
  InitializeWheel();
  
  if( Serial.available() )
  {
    
    int sel = Serial.parseInt();
    switch( sel )
    {
        case FORWARD:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, HIGH);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,HIGH);
           delay(3000);
           break;
           
        case BACKWARD:
           digitalWrite(LEFT_WHEEL_MINUS, HIGH);
           digitalWrite(LEFT_WHEEL_PLUS, LOW);
           digitalWrite(RIGHT_WHEEL_MINUS,HIGH);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           delay(3000);
           break;
           
        case TURN_LEFT:
           digitalWrite(LEFT_WHEEL_MINUS, LOW);
           digitalWrite(LEFT_WHEEL_PLUS, HIGH);
           digitalWrite(RIGHT_WHEEL_MINUS,LOW);
           digitalWrite(RIGHT_WHEEL_PLUS,LOW);
           delay(3000);
           break;
           
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
