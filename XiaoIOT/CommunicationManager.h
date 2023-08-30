#ifndef CommunicationManager_h
#define CommunicationManager_h

#include "Constants.h"
#include "Arduino.h"
#include "HardwareSerial.h"
#include "ScooterState.h"
#include "InternalState.h"
#include "Networking.h"

class CommunicationManager {
  public:
    enum Controller { ESC, BLE };

    CommunicationManager(HardwareSerial &escSerial, HardwareSerial &bleSerial);
    void recvd(int data, Controller src);
  
  private:
    int escStitch[MSG_SIZE_LIMIT] = {};
    int bleStitch[MSG_SIZE_LIMIT] = {};
    int escStitchPos = 0;
    int bleStitchPos = 0;
    HardwareSerial &escSerial;
    HardwareSerial &bleSerial;
    Networking networking;
    ScooterState scooterState;
    InternalState internalState;
    int test = 0;

    void rstPos(CommunicationManager::Controller src);
    int getPos(CommunicationManager::Controller src);
    void incPos(CommunicationManager::Controller src);
    void handle(int* message, int length, Controller src);
    void checkAndSend(int* message, int length, Controller dest);
    int getThrottle(int throttle);
    int getBrake(int brake);
    int getLight(int light);
    int getDisplayValue(int displayValue);
    int getLeds(int leds);
    void send(int* message, int length, Controller dest);
};

#endif