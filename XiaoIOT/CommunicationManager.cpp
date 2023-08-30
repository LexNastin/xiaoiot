// #include "wiring_time.h"
// #include <iterator>
#include "CommunicationManager.h"

#include "Constants.h"
#include "Arduino.h"
#include "HardwareSerial.h"
#include "ScooterState.h"
#include "InternalState.h"
#include "Networking.h"

CommunicationManager::CommunicationManager(HardwareSerial &escSerial, HardwareSerial &bleSerial)
  : escSerial(escSerial), bleSerial(bleSerial) {
  networking.init();
}

void CommunicationManager::rstPos(Controller src) {
  if (src == ESC) {
    escStitchPos = 0;
  } else if (src == BLE) {
    bleStitchPos = 0;
  }
}

int CommunicationManager::getPos(Controller src) {
  if (src == ESC) {
    return escStitchPos;
  } else if (src == BLE) {
    return bleStitchPos;
  }
}

void CommunicationManager::incPos(Controller src) {
  if (src == ESC) {
    escStitchPos++;
  } else if (src == BLE) {
    bleStitchPos++;
  }
}

void CommunicationManager::recvd(int data, Controller src) {
  int* stitch;
  if (src == ESC) {
    stitch = escStitch;
  } else if (src == BLE) {
    stitch = bleStitch;
  } else {
    return;
  }

  int curPos = getPos(src);

  // if this is true and we don't reset position, we're fucked. reset the position (don't won't a bof here, yk?)
  if (curPos >= MSG_SIZE_LIMIT) {
    rstPos(src);
    curPos = 0;
  }

  stitch[curPos] = data;
  switch(curPos) {
    case MAGIC_OFFSET_1:
      if (stitch[MAGIC_OFFSET_1] == MAGIC_1) {
        incPos(src);
        break;
      }
      rstPos(src);
      break;

    case MAGIC_OFFSET_2:
      if (stitch[MAGIC_OFFSET_2] == MAGIC_2) {
        incPos(src);
        break;
      }
      rstPos(src);
      break;

    case LENGTH_OFFSET:
      if (data <= MSG_SIZE_LIMIT - MSG_METADATA_SIZE) {
        incPos(src);
        break;
      }
      rstPos(src);
      break;

    default:
      incPos(src);

      int length = curPos + 1;
      // switch(src) {
      //   case ESC:
          // we should have all data. check checksum and parse message
          if (length == stitch[LENGTH_OFFSET] + MSG_METADATA_SIZE) {
            long sum = 0;
            for (int i = 2; i < length - MSG_END_METADATA; i++) {
              sum += stitch[i];
            }
            sum ^= 0xFFFF;
            int check1 = (sum & 0xFF00) >> 8;
            int check0 = (sum & 0x00FF) >> 0;
            if (stitch[length - 2] != check0) {
              rstPos(src);
              break;
            }
            if (stitch[length - 1] != check1) {
              rstPos(src);
              break;
            }


            // we actually now have the data and it's good. do whatev :)
            handle(stitch, length, src);

            // as a final, reset the stitch
            rstPos(src);
          }
          break;

      //   case BLE:
      //     if (curPos == stitch[LENGTH_OFFSET] + ESC_METADATA_SIZE - 1) {
      //       handle(stitch, curPos + 1, src);

      //       rstPos(src);
      //     }
      //     break;
      // }
      // break;
  }
}

void CommunicationManager::handle(int* message, int length, Controller src) {
  int dataLength = message[LENGTH_OFFSET];
  int dest = message[DESTINATION_OFFSET];
  int msgType = message[MESSAGE_TYPE_OFFSET];
  // TODO: remove
  if (msgType != 0x64 && msgType != 0x65) {
    delay(0);
  }
  // TODO: remove
  if (dest != 0x20) {
    delay(0);
  }
  int* data = message + DATA_OFFSET;

  // if (logFile) {
  //   logFile.printf("%d - ", millis());
  //   switch (src) {
  //     case ESC:
  //       logFile.print("ESC: 0x");
  //       break;

  //     case BLE:
  //       logFile.print("BLE: 0x");
  //       break;
  //   }
  //   for (int i = 0; i < length; i++) {
  //     logFile.printf("%x", message[i]);
  //   }
  //  logFile.println();

  //  if (millis() > 5000) {
  //    logFile.close();
  //  }
  // }

  int throttle;
  int brake;

  int flags;
  int leds;
  int light;
  int displayValue;
  int mode;

  if (src == BLE) {
    switch(msgType) {
      case TYPE_READ:
        break;

      case TYPE_WRITE:
        break;

      case TYPE_WRITE_REPLY:
        break;

      case TYPE_READ | SHFW:
        break;

      case TYPE_WRITE | SHFW:
        break;

      case TYPE_WRITE_REPLY | SHFW:
        break;

      // TODO: implement XIOT communication (ensure password protected, can be PT or hash)
      case TYPE_READ | XIOT:
        break;

      case TYPE_WRITE | XIOT:
        break;

      case TYPE_WRITE_REPLY | XIOT:
        break;
      
      case TYPE_STATUS:
        return;

      case TYPE_STATUS_REPLY:
        throttle = message[7];
        brake = message[8];

        scooterState.throttle = throttle;
        scooterState.brake = brake;

        message[7] = getThrottle(throttle);
        message[8] = getBrake(brake);

        checkAndSend(message, length - 2, ESC);
        return;

      case TYPE_READ_COMBO:
        throttle = message[8];
        brake = message[9];

        scooterState.throttle = throttle;
        scooterState.brake = brake;

        message[8] = getThrottle(throttle);
        message[9] = getBrake(brake);

        checkAndSend(message, length - 2, ESC);
        return;
        break;

      default:
        return;
    }
  }

  if (src == ESC) {
    switch(msgType) {
      case TYPE_STATUS_REPLY:
        flags = message[6];
        leds = message[7];
        light = message[8];
        displayValue = message[10];
        mode = (flags >> 1) & 0b11;

        scooterState.inMotion = flags & IN_MOTION; // TODO: same as isCharging - we could be in motion and not know that if we're in charge mode
        scooterState.isCharging = flags & CHARGING_MODE; // TODO: replace this with a more reliable method such as reading some register (in case user disabled charging mode)
        scooterState.leds = leds;
        scooterState.lightOn = light != 0;
        scooterState.displayValue = displayValue;
        switch(mode) {
          case 1: // eco
            scooterState.mode = 0;
            break;
          case 0: // drive
            scooterState.mode = 1;
            break;
          case 2: // sport
            scooterState.mode = 2;
            break;
        }

        message[7] = getLeds(leds);
        message[8] = getLight(light);
        message[10] = getDisplayValue(displayValue);

        // if (scooterState.brake <= 0x30) {
        //   message[7] = 40;
        //   message[8] = LIGHT_OFF;
        //   message[9] = 0x0;
        //   message[10] = 69;
        //   message[11] = 107; // error: A7 in this case
        //   checkAndSend(message, length - 2, BLE);
        //   return;
        // }
        checkAndSend(message, length - 2, BLE);
        return;
    }
  }

  switch(src) {
    case ESC:
      send(message, length, BLE);
      break;

    case BLE:
      send(message, length, ESC);
      break;
  }
  // test++;
  // int response[10 + MSG_END_METADATA] = {0x55, 0xaa, 0x06, 0x21, 0x64, 0x00, SPORT_MODE, 99, test % 2 == 0x0 ? LIGHT_ON : LIGHT_OFF, 0x00};
  // checkAndSend(response, 10, BLE);
}

// prevent acceleration and crap in this method
int CommunicationManager::getThrottle(int throttle) {
  return throttle;
}

// break brake
int CommunicationManager::getBrake(int brake) {
  return brake;
}

int CommunicationManager::getLight(int light) {
  return light;
}

int CommunicationManager::getDisplayValue(int displayValue) {
  return displayValue;
}

int CommunicationManager::getLeds(int leds) {
  return leds;
}

void CommunicationManager::checkAndSend(int* message, int length, Controller dest) {
  long sum = 0;
  for (int i = 2; i < length; i++) {
    sum += message[i];
  }
  sum ^= 0xFFFF;

  // adding check bytes to message in little endian ordering
  message[length + 0] = (sum & 0x00FF) >> 0;
  message[length + 1] = (sum & 0xFF00) >> 8;

  send(message, length + MSG_END_METADATA, dest);
}

void CommunicationManager::send(int* message, int length, Controller dest) {
  if (dest == ESC) {
    for (int i = 0; i < length; i++) {
      escSerial.write(message[i]);
    }
    escSerial.enableHalfDuplexRx();
  } else if (dest == BLE) {
    for (int i = 0; i < length; i++) {
      bleSerial.write(message[i]);
    }
    bleSerial.enableHalfDuplexRx();
  }
}
