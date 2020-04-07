#include <Arduino.h>

#include "BLEDevice.h"
int x_dat; // x Data
int y_dat; // y Data
int z_dat; // z Data
int z_button;
int c_button;
char* data;
unsigned long counter = 0;

//Get your UUID from Online UUID Generator.
//https://www.uuidgenerator.net/version4
#define SERVICE_UUID        "1812"
#define CHARACTERISTIC_UUID "2A4D"
#define DEVICE_NAME         "VR BOX"
#define LED      2

static BLEUUID  serviceUUID(SERVICE_UUID);
static BLEUUID  charUUID(CHARACTERISTIC_UUID);
static BLEAddress *pServerAddress;
static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
}

BLEClient*  pClient  = BLEDevice::createClient();
std::string old_value = "N/A" ;

bool connectToServer(BLEAddress pAddress) {
  Serial.print("Forming a connection to ");
  Serial.println(pAddress.toString().c_str());

  pClient->connect(pAddress);
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  Serial.print(" - Connected to server :");
  Serial.println(serviceUUID.toString().c_str());
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    return false;
  }
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  Serial.print(" - Found our characteristic UUID: ");
  Serial.println(charUUID.toString().c_str());
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    return false;
  }
  Serial.println(" - Found our characteristic");
  pRemoteCharacteristic->registerForNotify(notifyCallback);
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());
      Serial.println(advertisedDevice.getName().c_str());

      if (advertisedDevice.getName() == DEVICE_NAME) { 
        Serial.println(advertisedDevice.getAddress().toString().c_str());
        advertisedDevice.getScan()->stop();
        pServerAddress = new BLEAddress(advertisedDevice.getAddress());
        doConnect = true;
      }
    }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init(DEVICE_NAME);
  BLEScan* pBLEScan = BLEDevice::getScan();
  Serial.println("getScan");
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  Serial.println("setAdvertisedDeviceCallbacks");
  pBLEScan->setActiveScan(true);
  pBLEScan->start(10);
  Serial.println("End of setup");
  pinMode(LED,OUTPUT);



  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("We are now connected to the BLE Server.");
      connected = true;
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
      connected = false;
    }
    doConnect = false;
  }



}

void loop() {
  if (connected) {
    std::string value = pRemoteCharacteristic->readValue();
    if (old_value != value) {
      counter ++;
      Serial.println(String(counter) + ":" + value.c_str() );

      if (value == "BUTTON_PIN:1") {
        digitalWrite(LED,LOW);
      } else {
        digitalWrite(LED,HIGH);
      }
    }
    old_value = value;
  }
  delay(2);
  
}