#include <SPI.h>
#include <Servo.h>
#include "nRF24L01.h"
#include "RF24.h"

#define servoPin 3
#define servoMinImp 600                                                 // default 544
#define servoMaxImp 2456                                                // default 2400
#define motorMinImp 1000
#define motorMaxImp 2300
#define serialSpeed 9600 

RF24 radio(8,7);                                                        // "создать" модуль на пинах 9 и 10 Для Уно
Servo motorRR;                                                          // create a servo object
Servo motorFR;
Servo motorRL;
Servo motorFL;
Servo myServo;

byte address[][6] = {"1Node","2Node","3Node","4Node","5Node","6Node"};  //возможные номера труб
int valY = 0;
int valX = 0;
int coordinates[2] = {0, 0};

void setup() {
  Serial.begin(serialSpeed);

  motorRR.attach(10);                                                   //RR
  motorFR.attach(5);                                                    //FR
  motorRL.attach(6);                                                    //RL 
  motorFL.attach(9);                                                    //FL
  myServo.attach(servoPin, servoMinImp, servoMaxImp);
  
  motorRR.writeMicroseconds(motorMaxImp);
  motorFR.writeMicroseconds(motorMaxImp);
  motorRL.writeMicroseconds(motorMaxImp);
  motorFL.writeMicroseconds(motorMaxImp);
  delay(3000);
  
  motorRR.writeMicroseconds(motorMinImp);
  motorFR.writeMicroseconds(motorMinImp);
  motorRL.writeMicroseconds(motorMinImp);
  motorFL.writeMicroseconds(motorMinImp);
  myServo.write(90);
  delay(6000);  

  radio.begin();                                                        //активировать модуль
  radio.setAutoAck(1);                                                  //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0,15);                                               //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();                                             //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);                                             //размер пакета, в байтах

  radio.openReadingPipe(1,address[0]);                                  //хотим слушать трубу 0
  radio.setChannel(0x60);                                               //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX);                                       //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS);                                       //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
                                                                        //должна быть одинакова на приёмнике и передатчике!
                                                                        //при самой низкой скорости имеем самую высокую чувствительность и дальность!!
                                                                        // ВНИМАНИЕ!!! enableAckPayload НЕ РАБОТАЕТ НА СКОРОСТИ 250 kbps!
  
  radio.powerUp();                                                      //начать работу
  radio.startListening();                                               //начинаем слушать эфир, мы приёмный модуль
}

void loop(void) {
  byte pipeNo;   
  while( radio.available(&pipeNo)){                                     // слушаем эфир со всех труб
    radio.read( &coordinates, 32 );                                     // чиатем входящий сигнал      
    valX = constrain(map(coordinates[0], 800, 2300, motorMinImp, motorMaxImp), motorMinImp, motorMaxImp);  
    valY = constrain(map(coordinates[1], 1, 180, -1, 180), 0, 180);      
    myServo.write(valY);
        
    if(valY < 90) {                                                   // LEFT
      motorRL.writeMicroseconds(constrain((valX * ( valY / 90.0 )), motorMinImp, motorMaxImp));
      motorFL.writeMicroseconds(constrain((valX * ( valY / 90.0 )), motorMinImp, motorMaxImp));
      motorRR.writeMicroseconds(valX);
      motorFR.writeMicroseconds(valX);
    }
    else if(valY > 90) {                                              // RIGHT
      motorRL.writeMicroseconds(valX);
      motorFL.writeMicroseconds(valX);
      motorRR.writeMicroseconds(constrain((valX * ( map(valY, 91, 180, 89, 0) / 90.0 )), motorMinImp, motorMaxImp));
      motorFR.writeMicroseconds(constrain((valX * ( map(valY, 91, 180, 89, 0) / 90.0 )), motorMinImp, motorMaxImp));
    }
    else {
      motorRL.writeMicroseconds(valX);
      motorFL.writeMicroseconds(valX);
      motorRR.writeMicroseconds(valX);
      motorFR.writeMicroseconds(valX);
    }

    if(Serial.availableForWrite()) {
      Serial.print("Recieved: "); 
      Serial.print(valX);
      Serial.print(", ");
      Serial.print(valY);
      if(valY < 90) {   
        Serial.print(", L: ");
        Serial.print(constrain((valX * ( valY / 90.0 )), motorMinImp, motorMaxImp));
      }
      else if(valY > 90) {  
        Serial.print(", R: ");
        Serial.print(constrain((valX * ( map(valY, 91, 180, 89, 0) / 90.0 )), motorMinImp, motorMaxImp));
      }
      Serial.println("\r");    
    }    
  }  
}
