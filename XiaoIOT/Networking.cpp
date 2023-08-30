#include "Networking.h"

#include "Constants.h"
#include "SoftwareSerial.h"

SoftwareSerial cellular(CELLULAR_RX, CELLULAR_TX);

void Networking::init() {
  cellular.begin(9600);
}