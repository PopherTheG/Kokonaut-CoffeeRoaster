/***************************************************
   This is a program with a goal to make a quick prototype
   using Arduino libraries before transferring it and implementing
   it in the ESP-IDF.

   These uses SPI to communicate!
 ****************************************************/

#include <SPI.h>

#include "Adafruit_MAX31855.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


/* Defining pins to be usd for SPI communication */
#define MAXDO 3
#define MAXCS 4
#define MAXCLK 5

/* Initialize the Thermocouple */
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

/* */
BLECharacteristic *characteristicTX;

bool deviceConnected = false;

#define SERVICE_UUID   "ab0828b1-198e-4351-b779-901fa0e0371e"
#define CHARACTERISTIC_UUID_RX  "4ac8a682-9736-4e5d-932b-e9b31405049c"
#define CHARACTERISTIC_UUID_TX  "0972EF8C-7613-4075-AD52-756F33D4DA91"

class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    }
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) delay(1); // wait for Serial

  Serial.println("MAX31855 test");

  /* Wait for MAX chip to stabilize */
  delay(500);
  Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }

  /* Create the BLE Device */
  BLEDevice::init("ESP32-BLE");

  /* Create BLE Server */
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  /* Create the BLE Service */
  BLEService *service = server->createService(SERVICE_UUID);

  /* Create a BLE characteristic */
  characteristicTX = service->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  characteristicTX->addDescriptor(new BLE2902());

  /* Create a BLE characteristic */
  BLECharacteristic *characteristic = service->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

  characteristic->setCallbacks(new BLECharacteristicCallbacks());

  /* Start the service */
  service->start();

  /* Start advertising */
  server->getAdvertising()->start();

  Serial.println("DONE");
}

void loop() {
  float internal_temp = thermocouple.readInternal();
  float thermocouple_temp = thermocouple.readCelsius();
  if (isnan(thermocouple_temp)) {
    Serial.println("Something wrong with thermocouple");
  }
  if (deviceConnected)
  {
    char txString[8];
    dtostrf(thermocouple_temp, 2, 2, txString);

    characteristicTX->setValue(txString);
    characteristicTX->notify();
    Serial.println(txString);
  }
  delay(1000);
}

/* basic readout test, just print the current temp */
//  Serial.print("Internal Temp = ");
//  Serial.println(thermocouple.readInternal());
//
//  double c = thermocouple.readCelsius();
//  if (isnan(c)) {
//    Serial.println("Something wrong with thermocouple!");
//  } else  {
//    Serial.print("C = ");
//    Serial.println(c);
//  }
//  Serial.print("F = ");
//  Serial.println(thermocouple.readFahrenheit());
//  delay(1000);
