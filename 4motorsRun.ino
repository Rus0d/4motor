#include <Servo.h>

Servo motorRR;  // create a servo object
Servo motorFR;
Servo motorRL;
Servo motorFL;
String  direction = "NULL";
int valY = 0;
int valX = 0;
int val = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
    
  motorRR.attach(8); //RR
  motorFR.attach(9); //FR
  motorRL.attach(10); //RL 
  motorFL.attach(11); //FL
  
  motorRR.writeMicroseconds(2300);
  motorFR.writeMicroseconds(2300);
  motorRL.writeMicroseconds(2300);
  motorFL.writeMicroseconds(2300);
  delay(2000);
  
  motorRR.writeMicroseconds(800);
  motorFR.writeMicroseconds(800);
  motorRL.writeMicroseconds(800);
  motorFL.writeMicroseconds(800);
  delay(6000);
}

void loop() {
  valX = constrain(analogRead(0)-510, 0, 508);
  val = map(valX, 0, 508, 800, 2300);
  
  if(analogRead(1)<500) {
    direction = "LEFT";
    valY = map(analogRead(1), 0, 500, 2300, 0);
    motorRL.writeMicroseconds(val - valY);
    motorFL.writeMicroseconds(val - valY);
    motorRR.writeMicroseconds(val);
    motorFR.writeMicroseconds(val);
  }
  else if(analogRead(1)>500) {
    direction = "RIGHT";
    valY = map(analogRead(1), 500, 1018, 0, 2300);
    motorRL.writeMicroseconds(val);
    motorFL.writeMicroseconds(val);
    motorRR.writeMicroseconds(val - valY);
    motorFR.writeMicroseconds(val - valY);
  }
  else {
    direction = "NULL";
    valY = 0;
    motorRL.writeMicroseconds(val);
    motorFL.writeMicroseconds(val);
    motorRR.writeMicroseconds(val);
    motorFR.writeMicroseconds(val);
  }
//  Serial.print('X');
//  Serial.println(val);
  Serial.print(direction);
  Serial.println(valY);
}
