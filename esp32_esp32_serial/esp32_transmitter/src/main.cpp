#include <Arduino.h>
#include <HardwareSerial.h>

//HardwareSerial Serial2(2);
long counter = 0;

void setup() {
  Serial.begin(115200);
  Serial2.begin(19200, SERIAL_8N1, 16, 17);
}

void loop() {
  counter ++;
  Serial.println(counter);
  Serial2.println(counter);
  delay(1000);
}  