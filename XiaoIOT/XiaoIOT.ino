#include "Constants.h"
#include "CommunicationManager.h"

HardwareSerial ESCSerial(ESC_PIN);
HardwareSerial BLESerial(BLE_PIN);

CommunicationManager commsMgr(ESCSerial, BLESerial);

void setup() {
  // setting up intercept communication
  ESCSerial.begin(115200);
  BLESerial.begin(115200);

  pinMode(D2, INPUT_PULLUP);

  // // setting up logging
  // Serial.begin(115200);

  // // init messages
  // Serial.println("Hello, world!");
  // Serial.println("--- XiaoIOT by ArchGryphon9362 ---");
  // Serial.print("Version: ");
  // Serial.println(IOT_VER_STR);
  // BLESerial.write(65);
}

void loop() {
  int escAvailable = ESCSerial.available();
  for (int i = 0; i < escAvailable; i++) {
    int escByte = ESCSerial.read();
    commsMgr.recvd(escByte, CommunicationManager::Controller::ESC);
  }

  int bleAvailable = BLESerial.available();
  for (int i = 0; i < bleAvailable; i++) {
    int bleByte = BLESerial.read();
    commsMgr.recvd(bleByte, CommunicationManager::Controller::BLE);
  }

  // delay(100);
}