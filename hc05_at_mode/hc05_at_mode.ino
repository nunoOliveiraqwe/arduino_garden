#include <SoftwareSerial.h>

SoftwareSerial bt_serial(10,11);

#define DEFAULT_BAUD_RATE_BLUETOOTH_HC05 38400

void setup() {
  Serial.begin(DEFAULT_BAUD_RATE_BLUETOOTH_HC05);
  bt_serial.begin(DEFAULT_BAUD_RATE_BLUETOOTH_HC05);
  pinMode(10,INPUT);
  pinMode(11,OUTPUT);
}

void loop() {
 if(bt_serial.available()){
  int b = bt_serial.read();
  Serial.write((char)b);
 } else{
  //Serial.println("not available");
 }
 if(Serial.available()){
   int b = Serial.read();
    bt_serial.print((char)b);
  }
}
