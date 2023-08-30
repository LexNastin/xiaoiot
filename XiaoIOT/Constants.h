#ifndef Constants_h
#define Constants_h

// versions
#define IOT_VER 001
#define IOT_VER_STR "001"

// pins
#define ESC_PIN D5
#define BLE_PIN PA_2_ALT1 // this is A7, just using this pin name to avoid low power mode

#define CELLULAR_RX A2
#define CELLULAR_TX A3

// msg lengths
#define MSG_START_METADATA 4
#define MSG_END_METADATA 2
#define MSG_METADATA_SIZE MSG_START_METADATA + MSG_END_METADATA
#define MSG_SIZE_LIMIT 0xFF + MSG_METADATA_SIZE // this size allows for parsing of ANY message received (as the length is a single byte, limiting it to 0xFF)
#define ESC_END_METADATA 1

// magic bytes
#define MAGIC_1 0x55
#define MAGIC_2 0xAA

// msg offsets
#define MAGIC_OFFSET_1 0
#define MAGIC_OFFSET_2 1
#define LENGTH_OFFSET 2
#define DESTINATION_OFFSET 3
#define MESSAGE_TYPE_OFFSET 4
#define DATA_OFFSET 5

// scooter messages
// 0bXXXX:
//   MSB: charging mode
//   Middle 2: mode
//     0 - drive
//     1 - eco
//     2 - sport
//   LSB: in motion
#define ECO_MODE      0b0010
#define DRIVE_MODE    0b0000
#define SPORT_MODE    0b0100
#define IN_MOTION     0b0000
#define CHARGING_MODE 0b1000

#define LIGHT_OFF 0x00
#define LIGHT_ON 0x64

#define BEEP_OFF 0x00    // off
#define BEEP_SILENT 0x01 // single
#define BEEP_SINGLE 0x02 // long
#define BEEP_DOUBLE 0x03 // double
#define BEEP_LONG 0x04   // extra long

// message types
#define TYPE_READ 0x01
#define TYPE_WRITE_REPLY 0x02
#define TYPE_WRITE 0x03
#define SHFW 0x30 // "or" with this to make a r/w for shfw
#define XIOT 0x40 // "or" with this to make a r/w for xiaoiot
#define TYPE_STATUS 0x65
#define TYPE_STATUS_REPLY 0x64
#define TYPE_READ_COMBO 0x61

// eeprom
#define EEPROM_HEADER "X001"
#define EEPROM_BEGIN 0x04

#define PREF_OFFSET_LOCKED EEPROM_BEGIN
#define PREF_SIZE_LOCKED 1

#define PREF_OFFSET_PASSWD PREF_OFFSET_LOCKED + PREF_SIZE_LOCKED
#define PREF_SIZE_PASSWD 32 + 1 // 32 bytes + null byte ending

#define PREF_SIZE_TOTAL PREF_OFFSET_PASSWD + PREF_SIZE_PASSWD

#endif