// ESP32 BLE Server

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define HELLO_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define COUNT_UUID   "6f736fe9-ce83-4150-88d1-45881fb243cb"

static BLEServer *pServer = NULL;
static BLEService *pService = NULL;
static BLECharacteristic *pHelloCharacteristic = NULL;
static BLECharacteristic *pCountCharacteristic = NULL;
static BLEAdvertising *pAdvertising = NULL;

static bool deviceConnected = false;
static uint32_t count = 0;

class MyServerCallbacks: public BLEServerCallbacks 
{
    void onConnect(BLEServer* pServer) 
    {
      count = 0;
      deviceConnected = true;
      Serial.println("connected");
    };

    void onDisconnect(BLEServer* pServer) 
    {
      deviceConnected = false;
      Serial.println("disconnected");
    }
};

class HelloCallbacks: public BLECharacteristicCallbacks 
{
	void onRead(BLECharacteristic* pCharacteristic)
    {
        Serial.println("hello read");
    }

    void onWrite(BLECharacteristic *pCharacteristic) 
    {
      std::string value = pCharacteristic->getValue();
      if (!value.empty()) 
      {
        Serial.print("hello written: ");
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
  pService = pServer->createService(SERVICE_UUID);

  // create BLE characteristic
  pHelloCharacteristic = pService->createCharacteristic(
      HELLO_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE
    );
  pHelloCharacteristic->setValue("Hello World!");
  pHelloCharacteristic->setCallbacks(new HelloCallbacks());

  // create another BLE characteristic
  pCountCharacteristic = pService->createCharacteristic(
      COUNT_UUID,
      BLECharacteristic::PROPERTY_NOTIFY
    );
  pCountCharacteristic->setValue(count);
  pCountCharacteristic->addDescriptor(new BLE2902()); // CCCD

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
  count++;

  if (deviceConnected)
  {
    pCountCharacteristic->setValue(count);
    pCountCharacteristic->notify();
  }

  delay(10);
}
