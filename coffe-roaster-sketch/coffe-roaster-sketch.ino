/***************************************************
   This is a program with a goal to make a quick prototype
   using Arduino libraries before transferring it and implementing
   it in the ESP-IDF.

   These uses SPI to communicate!
 ****************************************************/

#include <SPI.h>

#include "Adafruit_MAX31855.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

/* Defining pins to be usd for SPI communication */
#define MAXDO 3
#define MAXCS 4
#define MAXCLK 5

/* Initialize the Thermocouple */
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

BLECharacteristic *pCharacteristic;
char tempDataBuf[20];
bool deviceConnected = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("MAX31855 test");

  BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");

  /* Wait for MAX chip to stabilize */
  delay(500);
  Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
}

void loop() {
  /* basic readout test, just print the current temp */
  Serial.print("Internal Temp = ");
  Serial.println(thermocouple.readInternal());

  double c = thermocouple.readCelsius();
  if (isnan(c)) {
    Serial.println("Something wrong with thermocouple!");
  } else  {
    Serial.print("C = ");
    Serial.println(c);
  }
  Serial.print("F = ");
  Serial.println(thermocouple.readFahrenheit());
  sprintf(tempDataBuf, "%lf %lf", thermocouple.readInternal(), thermocouple.readCelsius());
  pCharacteristic->setValue(tempDataBuf);
  delay(1000);
}
