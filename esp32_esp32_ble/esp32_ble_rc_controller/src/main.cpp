/*
 ===================================
  ESP32 BLE Controller  
   - Sending data to Client 

 ===================================
*/


#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED  2
#define BUTTON_PIN   15
#define SIGNAL_CYCLE 50
#define MSG_LEN      120

/*
 ===================================
  Define BLE 
 ===================================
*/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" 
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define DEVIDE_NAME            "ESP32_BLE_CNTLR"




BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

float txValue = 0;


unsigned long counter = 0;
char ble_message[MSG_LEN];
String message;



class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Create the BLE Device
  BLEDevice::init(DEVIDE_NAME); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
                      
  pCharacteristic->addDescriptor(new BLE2902());
  
  //pCharacteristic->setCallbacks(new BLECallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    counter ++;

    //("BUTTON_PIN:" + String(digitalRead(BUTTON_PIN))).toCharArray(message, 16);

    message = "LBTN:" + String(digitalRead(BUTTON_PIN)) + ",LX:100,LY:100,RBTN:1,RX:100,RY:100,MPU_AX:9999,MPU_AY:9999,MPU_AZ:9999,MPU_GX:9999,MPU_GY:9999,MPU_GZ:9999";
    message.toCharArray(ble_message, 120);

    pCharacteristic->setValue(ble_message);
    pCharacteristic->notify(); // Send the value to the app!
    //pCharacteristic->indicate(); // Send the value to the app!
 
    Serial.println("*** Sent Value: " +  String(counter) );
    if (txValue == 0.00) {
      digitalWrite(LED,LOW);
    } else {
      digitalWrite(LED,HIGH);
    }

  } else {

    Serial.println("Disconnected.... ");
  }
  delay(SIGNAL_CYCLE);
}
 


