#include "InternalState.h"

#include "Constants.h"
#include "EEPROM.h"

InternalState::InternalState() {
  bool ready = true;

  for (int i = 0; i < EEPROM_BEGIN; i++) {
    if (EEPROM.read(i) == EEPROM_HEADER[i]) continue;

    ready = false;
    break;
  }

  if (!ready) {
    for (int i = 0; i < PREF_SIZE_TOTAL; i++) {
      if (i < EEPROM_BEGIN) {
        EEPROM.update(i, EEPROM_HEADER[i]);
        continue;
      }
      EEPROM.update(i, 0);
    }
  }
}

bool InternalState::getLocked() {
  EEPROM.read(PREF_OFFSET_LOCKED);
}

char* InternalState::getPassword() {
  // TODO: it is late, i can't be fucked. will fix next opportunity i have
  for (int i = PREF_OFFSET_PASSWD; i < PREF_OFFSET_PASSWD + PREF_SIZE_PASSWD; i++) {
    return "";
  }
  return "";
}