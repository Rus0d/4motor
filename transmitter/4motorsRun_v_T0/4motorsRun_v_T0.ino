#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);                                                       // "создать" модуль на пинах 9 и 10 Для Уно

byte address[][6] = {"1Node","2Node","3Node","4Node","5Node","6Node"};  //возможные номера труб

int coordinates[2] = {0, 0};

void setup(){
  Serial.begin(9600);                                                   //открываем порт для связи с ПК
  delay(2000);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);

  radio.begin();                                                        //активировать модуль
  radio.setAutoAck(1);                                                  //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0,15);                                               //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();                                             //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);                                             //размер пакета, в байтах

  radio.openWritingPipe(address[0]);                                    // мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);                                               //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX);                                       //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS);                                       //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
                                                                        //должна быть одинакова на приёмнике и передатчике!
                                                                        //при самой низкой скорости имеем самую высокую чувствительность и дальность!!
                                                                        // ВНИМАНИЕ!!! enableAckPayload НЕ РАБОТАЕТ НА СКОРОСТИ 250 kbps!

  radio.powerUp();                                                      //начать работу
  radio.stopListening();                                                //не слушаем радиоэфир, мы передатчик
}

void loop(void) {    
  coordinates[0] = constrain(map(analogRead(0), 1000, 0, -1500, 1500) + map(analogRead(2), 0, 1023, -750, +750), -1500, 1500);
  if (coordinates[0] >= 0) {
    coordinates[0] = coordinates[0] + 800;
  }
  else {
    coordinates[0] = coordinates[0] - 800;
  }
  coordinates[1] = constrain(map(analogRead(1), 0, 1018, 0, 180) + map(analogRead(7), 0, 1023, -45, 45), map(analogRead(6), 0, 1023, 0, 45), 180 - map(analogRead(6), 0, 1023, 0, 45));
  
  Serial.print(analogRead(0));
  Serial.print(", ");
  Serial.print(analogRead(1));
  Serial.print(", ");  
  Serial.print(analogRead(2));
  Serial.print(", ");  
  Serial.print(analogRead(6));
  Serial.print(", ");
  Serial.print(analogRead(7));
  Serial.print(", ");
  Serial.print(coordinates[0]);
  Serial.print(", ");
  Serial.println(coordinates[1]);
  radio.write(&coordinates, 32);  
}
