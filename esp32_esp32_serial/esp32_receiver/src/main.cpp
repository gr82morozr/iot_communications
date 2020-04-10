#include <Arduino.h>
#include <HardwareSerial.h>

//HardwareSerial Serial2(2);

void setup() {
  Serial.begin(115200);
  Serial2.begin(19200, SERIAL_8N1, 16, 17);
}

void loop() {
  String received = "";
  while (Serial2.available())   {
    received = Serial2.readString();
    Serial2.println(received);
    Serial.println(received);
    delay(10);
  }
}