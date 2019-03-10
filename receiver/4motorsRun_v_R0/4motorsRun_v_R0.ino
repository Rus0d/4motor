#include <SPI.h>
#include <Servo.h>
#include "nRF24L01.h"
#include "RF24.h"

#define servoPin 3
#define servoMinImp 600                                                 // default 544
#define servoMaxImp 2456                                                // default 2400

RF24 radio(8,7);                                                       // "создать" модуль на пинах 9 и 10 Для Уно
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
  Serial.begin(9600);

  motorRR.attach(10);                                                    //RR
  motorFR.attach(5);                                                    //FR
  motorRL.attach(6);                                                    //RL 
  motorFL.attach(9);                                                    //FL
  myServo.attach(servoPin, servoMinImp, servoMaxImp);
  
  motorRR.writeMicroseconds(2300);
  motorFR.writeMicroseconds(2300);
  motorRL.writeMicroseconds(2300);
  motorFL.writeMicroseconds(2300);
  delay(2000);
  
  motorRR.writeMicroseconds(800);
  motorFR.writeMicroseconds(800);
  motorRL.writeMicroseconds(800);
  motorFL.writeMicroseconds(800);
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
    while( radio.available(&pipeNo)){                                   // слушаем эфир со всех труб
      radio.read( &coordinates, 32 );                                   // чиатем входящий сигнал
//      Serial.print("Recieved: "); 
//      Serial.print(coordinates[0]);
//      Serial.print(", ");
//      Serial.println(coordinates[1]);  
   }
  
  valX = coordinates[0];  
  valY = coordinates[1];
  
  myServo.write(valY);
  
  if(valY < 90) {                                              // LEFT
    Serial.print("Left, ");    
    motorRL.writeMicroseconds(valX * valY / 90);
    motorFL.writeMicroseconds(valX * valY / 90);
    motorRR.writeMicroseconds(valX);
    motorFR.writeMicroseconds(valX);
  }
  else if(valY > 90) {                                         // RIGHT
    Serial.print("Right, ");
    motorRL.writeMicroseconds(valX);
    motorFL.writeMicroseconds(valX);
    motorRR.writeMicroseconds(valX * (180 - valY) / 90);
    motorFR.writeMicroseconds(valX * (180 - valY) / 90);
  }
  else {
    motorRL.writeMicroseconds(valX);
    motorFL.writeMicroseconds(valX);
    motorRR.writeMicroseconds(valX);
    motorFR.writeMicroseconds(valX);
  }
  
  Serial.print("X: ");
  Serial.print(valX);
  Serial.print(", ");
  Serial.print("Y: ");
  Serial.println(valY);
}
