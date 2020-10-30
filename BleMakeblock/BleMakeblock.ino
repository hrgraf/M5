#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic *pCharacteristic = NULL;
BLEAdvertising *pAdvertising = NULL;

bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks 
{
    void onConnect(BLEServer* pServer) 
    {
      deviceConnected = true;
      Serial.println("connected");
    };

    void onDisconnect(BLEServer* pServer) 
    {
      deviceConnected = false;
      Serial.println("disconnected");
    }
};

class MyCallbacks: public BLECharacteristicCallbacks 
{
	void onRead(BLECharacteristic* pCharacteristic)
    {
        Serial.println("read");
    }

    void onWrite(BLECharacteristic *pCharacteristic) 
    {
      std::string value = pCharacteristic->getValue();
      if (!value.empty()) 
      {
        Serial.print("written: ");
        Serial.println(value.c_str());
      }
    }
};

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting BLE server");

  // create BLE device
  BLEDevice::init("ESP32_BLE_Server");

  // create BLE server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // create BLE service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // create BLE characteristic
  pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE
                    );
  pCharacteristic->setValue("Hello World!");
  pCharacteristic->setCallbacks(new MyCallbacks());

  // start BLE service
  pService->start();

  // start BLE advertising
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();
  Serial.println("Waiting for client");
}

void loop() 
{
  delay(10);
}
