#include <EEPROM.h>
#include "password.h"
#include "keypad.h"
#include "buzzer.h"
#include "door.h"
#include "dc_motor.h"
//********************************************************//
#include <SPI.h>
#include <MFRC522.h>
#include "Arduino.h"
#include "RTClib.h"
RTC_DS1307 rtc;
//********************************************************//
/**************************************************************/
#define TYPE_BYTE_NUM 0
#define ID_BYTE_NUM 3
#define MONTH_DAY_BYTE_NUM 7
#define YEAR_BYTE_NUM 8
#define MINET_BYTE_NUM 9
#define HOUR_BYTE_NUM 10
#define ROOM_BYTE_NUM 11
#define FLOOR_BYTE_NUM 12
#define BUILDING_BYTE_NUM 13
#define CRC_BYTE_NUM 15
#define SYNC_MINET_BYTE_NUM 13
#define SYNC_HOUR_BYTE_NUM 10
#define SYNC_MONTH_DAY_BYTE_NUM 11
#define SYNC_YEAR_BYTE_NUM 12
/*************************************************************/
#define PROGRAMMIN_PERIOD 60000  // mill second
/*************************************************************/
/**************************************************************/
#define EEPROM_ROOM_BYTE_NUM 8
#define EEPROM_FLOOR_BYTE_NUM 9
#define EEPROM_BUILDING_BYTE_NUM 10
/**************************************************************/
#define RST_PIN 9  // Configurable, see typical pin layout above
#define SS_PIN 10  // Configurable, see typical pin layout above
/**************************************************************/
#define SECONDS 0
#define DAYOFWEEK 1
/**************************************************************/
#define SECONDS_FROM_1970_TO_2000 946684800
const byte daysInMonth[11] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 };
/************************RFID VARIABLES****************************/
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
MFRC522::MIFARE_Key key;
/***********************************************************/
typedef struct
{
  byte Room;
  byte Floor;
  byte Building;
} tLocation;  // UnitLocation
typedef struct
{
  byte Minute;
  byte Hour;
  byte Day;
  byte Month;
  byte Year;
} tDate;  //CurrentDate ,SyncDate
typedef struct
{
  byte Type;
  byte ID;
  tDate ExpirdDate;
  tLocation CardLocation;
  byte CRC;
} tCard;  // currentCard ,prevCard

typedef enum {
  AUTHORISED = 0x00,
  ROOM = 0x08,
  TIMESYNC = 0x0C,
  //FLOOR      = 0x30,
  //BUILDING   = 0x34,
  MASTER = 0x2E,
  EMRGANCY = 0x29,
  CLIENT = 0x18,
  OTHER = 0xFF
} tType;

typedef enum {
  INVALID_STATE,
  VALID_STATE
} tUserState;
typedef enum {
  IDEAL,
  PROGRAMMING_CARD,
  PROGRAMMING_PASS,
  LIGHT_ONLY,
  ALL_OUTPUTS
} tMode;
/************************Variables definetions**********************************/
tMode g_mode = IDEAL;
tUserState g_user_state = INVALID_STATE;
tCard g_Current_card, g_Prev_card;
tLocation g_unit_location;
byte g_buffer[18], g_prev_buffer[18], g_buffer_size = sizeof(g_buffer);
tDate g_current_date, g_sync_date;
unsigned long g_authorised_period, g_authorised_data, g_light_only_last_date, g_light_only_start_date, g_all_outputs_date;
unsigned long g_current_unixtime, g_expired_unixtime;
/*******************************************************************************/
void Password_Init() {
  // Read saved client and master passwords from EEPROM
  for (uint8 i = 0; i < PASSWORD_SIZE; i++) {
    eg_savedPass[i] = EEPROM.read(EEPROM_CLIENT_PASS_FIRST_BYTR + i);
    eg_masterPass[i] = EEPROM.read(EEPROM_MASTER_PASS_FIRST_BYTR + i);
  }
  return;
}
// Update password in EEPROM
void Password_Update(uint8* ptrTopass, uint8 l_eeprom_first_byte) {
  // Write password to specified EEPROM location
  for (uint8 i = 0; i < PASSWORD_SIZE; i++) {
    EEPROM.write(l_eeprom_first_byte + i, ptrTopass[i]);
  }
  return;
}

void setup() {
  Serial.begin(115200); // Initialize serial communications at 115200 bps
  Buzzer_Init();
  Password_Init();
  RTC_Init();
  CARD_Init();
  RFID_Init();
  CHECK_VALIDATION_Init();
  Door_Init();
  DC_MOTOR_Init();
//  Lock_Init();
  // uncomment to set unit location manual

  g_unit_location.Room = 5;
  g_unit_location.Floor = 1;
  g_unit_location.Building = 2;
  UNIT_Update();
  UNIT_Init_Read();
  MODE_Init();
  Door_Unlock();
  delay(3000);
}

void loop() {
  CARD_Update();
  //CardDataDisplay();
  RTC_Update();
  //RealTimeDisplay();
  CHECK_VALIDATION_Update();
  Door_Update();
  DoorDisplay();
  Buzzer_Update();
  //MODE_Update();
}

void CARD_Init(void) {
  g_Current_card.Type = OTHER;
  g_Current_card.ID = 0x00;
  g_Current_card.ExpirdDate.Minute = 0x00;
  g_Current_card.ExpirdDate.Hour = 0x00;
  g_Current_card.ExpirdDate.Day = 0x00;
  g_Current_card.ExpirdDate.Month = 0x00;
  g_Current_card.ExpirdDate.Year = 0x00;
  g_Current_card.CardLocation.Room = 0xFF;
  g_Current_card.CardLocation.Floor = 0xFF;
  g_Current_card.CardLocation.Building = 0xFF;
  g_Current_card.CRC = 0xFF;

}
void CARD_Update(void) {
  byte temp;
  /*****************read the  new card*********************/
  Serial.println("ReadCardHere");
  CardRead();
  /*****************Extract data***************************/
  temp = g_buffer[TYPE_BYTE_NUM];
  switch (temp) {
    case 0x00:
      g_Current_card.Type = AUTHORISED;
      break;
    case 0x08:
      g_Current_card.Type = ROOM;
      break;
    case 0x0C:
      g_Current_card.Type = TIMESYNC;
      break;
    /*    case 0x30:
      g_Current_card.Type = FLOOR;
      break;
      case 0x34:
      g_Current_card.Type = BUILDING;
      break;
    */
    case 0x2E:
      g_Current_card.Type = MASTER;
      break;

    case 0x29:
      g_Current_card.Type = EMRGANCY;
      break;
    case 0x18:
      g_Current_card.Type = CLIENT;
      break;
    default:
      g_Current_card.Type = OTHER;
      break;
  }
  g_Current_card.ID = g_buffer[ID_BYTE_NUM];
  g_Current_card.ExpirdDate.Minute = (g_buffer[MINET_BYTE_NUM] & 0x7f);
  g_Current_card.ExpirdDate.Hour = (((g_buffer[MINET_BYTE_NUM] & 0x80) >> 3) | (g_buffer[HOUR_BYTE_NUM] >> 4));
  g_Current_card.ExpirdDate.Day = (g_buffer[MONTH_DAY_BYTE_NUM] & 0x0f) | ((g_buffer[YEAR_BYTE_NUM] & 0x80) >> 3);
  g_Current_card.ExpirdDate.Month = (g_buffer[MONTH_DAY_BYTE_NUM] >> 4);
  g_Current_card.ExpirdDate.Year = (g_buffer[YEAR_BYTE_NUM] & 0x7f);
  g_Current_card.CardLocation.Room = g_buffer[ROOM_BYTE_NUM];
  g_Current_card.CardLocation.Floor = g_buffer[FLOOR_BYTE_NUM];
  g_Current_card.CardLocation.Building = g_buffer[BUILDING_BYTE_NUM];
  g_Current_card.CRC = g_buffer[CRC_BYTE_NUM];
  g_expired_unixtime = UnixTime();

}
void CHECK_VALIDATION_Init(void) {
  g_user_state = INVALID_STATE;
}
void CHECK_VALIDATION_Update(void) {
  if (g_Current_card.Type == OTHER) {
    Serial.println("INVALID_STATE1");
    g_user_state = INVALID_STATE;
    return;
  } else {
    Serial.print(g_expired_unixtime);
    Serial.print("   ");
    Serial.println(g_current_unixtime);
    if (g_expired_unixtime >= g_current_unixtime)  // check the date
    {
      Serial.println("date done");
      if ((g_Current_card.Type == AUTHORISED) || (g_Current_card.Type == ROOM) || (g_Current_card.Type == TIMESYNC)) {
        Serial.println("AUTHORISED-ROOM_TIMESYNC done");
        g_user_state = VALID_STATE;
        return;
      } else if ((g_Current_card.Type == MASTER) || (g_Current_card.Type == EMRGANCY)) {
        Serial.println("MASTER or EMRGANCY");
        g_user_state = VALID_STATE;
        return;
      } else if (g_Current_card.Type == CLIENT) {
        if (g_Current_card.CardLocation.Room == g_unit_location.Room) {
          Serial.println("Room done");
          if (g_Current_card.CardLocation.Floor == g_unit_location.Floor) {
            Serial.println("floor done");
            if (g_Current_card.CardLocation.Building == g_unit_location.Building) {
              Serial.println("buiding done");
              g_user_state = VALID_STATE;
              return;
            }
          }
        }
      }
    }
  }

  Serial.println("INVALID_STATE");
  g_user_state = INVALID_STATE;
  return;
}
void CardRead(void) {
  for (int i = 0; i < g_buffer_size; i++) {
    g_prev_buffer[i] = g_buffer[i];
  }
  static byte s_NoCardCounter = 0;
  if (!mfrc522.PICC_IsNewCardPresent()) {
    s_NoCardCounter++;
    if (s_NoCardCounter > 5) {
      for (int i = 0; i < g_buffer_size; i++) {
        g_buffer[i] = 0xff;
      }

      s_NoCardCounter = 0;
    }
    //Serial.println("isNewCardPresent");
    //Serial.println(s_NoCardCounter);
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    //Serial.println("ReadCardSerial");
    return;
  }
  s_NoCardCounter = 0;
  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F("Card UID:"));
  //dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Check for compatibility
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI
      && piccType != MFRC522::PICC_TYPE_MIFARE_1K
      && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("This sample only works with MIFARE Classic cards."));
    return;
  }

  // In this sample we use the second sector,
  // that is: sector #1, covering block #4 up to and including block #7
  byte sector = 4;
  byte blockAddr = 16;
  byte dataBlock[] = {
    0x01, 0x02, 0x03, 0x04,  //  1,  2,   3,  4,
    0x05, 0x06, 0x07, 0x08,  //  5,  6,   7,  8,
    0x09, 0x0a, 0xff, 0x0b,  //  9, 10, 255, 11,
    0x0c, 0x0d, 0x0e, 0x0f   // 12, 13, 14, 15
  };
  byte trailerBlock = 7;
  MFRC522::StatusCode status;


  // Authenticate using key A
  Serial.println(F("Authenticating using key A..."));
  status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Show the whole sector as it currently is
  Serial.println(F("Current data in sector:"));
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();

  // Read data from the block
  Serial.print(F("Reading data from block "));
  Serial.print(blockAddr);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, g_buffer, &g_buffer_size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    for (int i = 0; i < g_buffer_size; i++) {
      g_buffer[i] = g_prev_buffer[i];
    }
  }
  Serial.print(F("Data in block "));
  Serial.print(blockAddr);
  Serial.println(F(":"));
  Serial.println();
  mfrc522.PCD_StopCrypto1();
}
void MODE_Init(void) {
  g_mode = IDEAL;
}
/*void MODE_Update(void) {

  switch (g_user_state) {
    case INVALID_STATE:
      switch (g_mode) {
        case IDEAL:
          g_mode = IDEAL;
          Serial.println("INVALEDSTATE_IN_MODE");
          break;
        case PROGRAMMING:
          if ((millis() - g_authorised_data) > (PROGRAMMIN_PERIOD)) {
            BUZZER_Flush(PROGRAMMING);
            g_mode = IDEAL;
            Serial.println("INVALEDSTATE_IN_AUTHORISED");
          }
          break;
        case LIGHT_ONLY:
          if ((millis() - g_light_only_last_date) > (OFF_DELAY)) {
            g_mode = IDEAL;
            Serial.println("INVALEDSTATE_IN_LIGHT_ONLY");
          }
          break;
        case ALL_OUTPUTS:
          if ((millis() - g_all_outputs_date) > (OFF_DELAY)) {
            g_mode = IDEAL;
            Serial.println("INVALEDSTATE_IN_ALL_OUTPUTS");
          }
          break;
        default:
          g_mode = IDEAL;
          Serial.println("INVALEDSTATE_IN_DEFAULT");
          break;
      }
      break;

    case VALID_STATE:
      {
        Serial.println("VALEDSTATEINMODE");
        switch (g_mode) {
          case IDEAL:
            Serial.println("IDEAL");
            switch (g_Current_card.Type) {
              case AUTHORISED:
                Serial.println("AUTHORISED");
                g_mode = PROGRAMMING;
                BUZZER_Flush(AUTHORISED);
                g_authorised_data = millis();  // --------------------->>
                Serial.println("PROGRAMMING1");
                break;
                //  case MASTER:
              case EMRGANCY:
                // case BUILDING:
                // case FLOOR:
              case CLIENT:
                Serial.println("MASTER-EMRGANCY-BUILDING-FLOOR-CLIENT");
                g_mode = LIGHT_ONLY;
                g_light_only_start_date = millis();  // --------------------->>
                break;
              default:
                break;
            }
            break;
          case PROGRAMMING:
            Serial.println("PROGRAMMING2");
            if ((millis() - g_authorised_data) <= (PROGRAMMIN_PERIOD)) {
              switch (g_Current_card.Type) {
                Serial.println("PROGRAMMING3");
                case ROOM:
                  Serial.println("ROOM");
                  g_unit_location.Room = g_buffer[ROOM_BYTE_NUM];
                  g_unit_location.Floor = g_buffer[FLOOR_BYTE_NUM];
                  g_unit_location.Building = g_buffer[BUILDING_BYTE_NUM];
                  UNIT_Update();
                  BUZZER_Flush(ROOM);
                  g_mode = IDEAL;
                  break;
                case TIMESYNC:
                  Serial.println("TIMESYNC");
                  g_sync_date.Minute = (g_buffer[SYNC_MINET_BYTE_NUM] & 0x7f);
                  g_sync_date.Hour = (((g_buffer[SYNC_MINET_BYTE_NUM] & 0x80) >> 3) | (g_buffer[SYNC_HOUR_BYTE_NUM] & 0x0f));
                  g_sync_date.Day = (g_buffer[SYNC_MONTH_DAY_BYTE_NUM] & 0x0f) | ((g_buffer[SYNC_YEAR_BYTE_NUM] & 0x80) >> 3);
                  g_sync_date.Month = (g_buffer[SYNC_MONTH_DAY_BYTE_NUM] >> 4);
                  g_sync_date.Year = (g_buffer[SYNC_YEAR_BYTE_NUM] & 0x7f);
                  RTC_Set();
                  BUZZER_Flush(TIMESYNC);
                  g_mode = IDEAL;
                  break;
                default:
                  break;
              }
            } else {
              Serial.println("PROGRAMMING4");
              BUZZER_Flush(PROGRAMMING);
              g_mode = IDEAL;
            }
            break;
          case LIGHT_ONLY:
            Serial.println("LIGHT_ONLY");
            g_light_only_last_date = millis();  // --------------------->>
            switch (g_Current_card.Type) {
              case CLIENT:
              case EMRGANCY:
             // case MASTER:
                Serial.println("CLIENT-EMRGANCY-MASTER");

                if ((millis() - g_light_only_start_date) >= (AC_DELAY)) {
                  g_mode = ALL_OUTPUTS;
                }
                break;
              default:
                break;
            }
            break;
          case ALL_OUTPUTS:
            switch (g_Current_card.Type) {
              case CLIENT:
              case EMRGANCY:
              //case MASTER:
                g_all_outputs_date = millis();  //------------------------------------>
                Serial.println("ALLOUTPUTS");
                break;
              default:
                g_mode = IDEAL;
            }
            break;
          default:
            g_mode = IDEAL;
            break;
        }
      }
      break;
    default:
      g_mode = IDEAL;
      break;
  }
  }
*/
void RFID_Init(void) {
  while (!Serial)
    ;                  // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}
void UNIT_Init_Read(void) {
  g_unit_location.Room = EEPROM.read(EEPROM_ROOM_BYTE_NUM);
  g_unit_location.Floor = EEPROM.read(EEPROM_FLOOR_BYTE_NUM);
  g_unit_location.Building = EEPROM.read(EEPROM_BUILDING_BYTE_NUM);
}
void UNIT_Update(void) {
  EEPROM.write(EEPROM_ROOM_BYTE_NUM, g_unit_location.Room);
  EEPROM.write(EEPROM_FLOOR_BYTE_NUM, g_unit_location.Floor);
  EEPROM.write(EEPROM_BUILDING_BYTE_NUM, g_unit_location.Building);
}
void RTC_Set(void) {
  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  rtc.adjust(DateTime(g_sync_date.Year, g_sync_date.Month, g_sync_date.Day, g_sync_date.Hour, g_sync_date.Minute, 0));
  // delay(3000); // wait for console opening
  //URTCLIB_WIRE.begin();
}
void RTC_Update(void) {
  DateTime now = rtc.now();
  //Serial.print(" since midnight 1/1/1970 = ");
  //Serial.print(now.unixtime());
  //Serial.print("s");
  g_current_unixtime = now.unixtime();
  g_current_date.Minute = now.minute();
  g_current_date.Hour = now.hour();
  g_current_date.Day = now.day();
  g_current_date.Month = now.month();
  g_current_date.Year = now.year() - 2000;
}

void RTC_Init(void) {
#ifndef ESP8266
  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB
#endif

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  //rtc.adjust(DateTime(2024, 1, 1, 0, 0, 0));
  //delay(3000); // wait for console opening
  //URTCLIB_WIRE.begin();
}
unsigned long UnixTime(void) {
  unsigned long t;
  unsigned int days = Date2Days(g_Current_card.ExpirdDate.Year, g_Current_card.ExpirdDate.Month, g_Current_card.ExpirdDate.Day);
  t = Time2Ulong(days, g_Current_card.ExpirdDate.Hour, g_Current_card.ExpirdDate.Minute);
  t += SECONDS_FROM_1970_TO_2000;  // seconds from 1970 to 2000

  return t;
}
unsigned int Date2Days(unsigned int y, byte m, byte d) {
  if (y >= 2000U)
    y -= 2000U;
  unsigned int days = d;
  for (byte i = 1; i < m; ++i) {
    days += daysInMonth[i - 1];
  }
  if (m > 2 && y % 4 == 0)  // if current year is leap
    ++days;
  return days + 365 * y + (y + 3) / 4 - 1;  // num of previce leap years
}
unsigned long Time2Ulong(unsigned int days, byte h, byte m) {
  return ((days * 24UL + h) * 60 + m) * 60;
}
/**************************testing function*******************************/
const char* CardTypeToString(byte type) {
  switch (type) {
    case AUTHORISED: return "AUTHORISED";
    case ROOM: return "ROOM";
    case TIMESYNC: return "TIMESYNC";
    // case FLOOR: return "FLOOR";
    //case BUILDING: return "BUILDING";
    case MASTER: return "MASTER";
    case EMRGANCY: return "EMERGENCY";
    case CLIENT: return "CLIENT";
    default: return "OTHER";
  }
}

void CardDataDisplay() {
  Serial.print("TYPE = ");
  Serial.print(CardTypeToString(g_Current_card.Type));
  Serial.print(" ID = ");
  Serial.print(g_Current_card.ID);

  Serial.print(" | DATE = ");
  Serial.print(g_Current_card.ExpirdDate.Year);
  Serial.print("-");
  Serial.print(g_Current_card.ExpirdDate.Month);
  Serial.print("-");
  Serial.print(g_Current_card.ExpirdDate.Day);

  Serial.print(" | TIME = ");
  Serial.print(g_Current_card.ExpirdDate.Hour);
  Serial.print(":");
  Serial.println(g_Current_card.ExpirdDate.Minute);

  Serial.print("Room: ");
  Serial.print(g_Current_card.CardLocation.Room);
  Serial.print(", Floor: ");
  Serial.print(g_Current_card.CardLocation.Floor);
  Serial.print(", Building: ");
  Serial.println(g_Current_card.CardLocation.Building);

  Serial.print("CRC = ");
  Serial.println(g_Current_card.CRC);
}
void RealTimeDisplay() {
  Serial.print("Current Time = ");
  if (g_current_date.Hour < 10) Serial.print("0");
  Serial.print(g_current_date.Hour);
  Serial.print(":");
  if (g_current_date.Minute < 10) Serial.print("0");
  Serial.print(g_current_date.Minute);

  Serial.print(" | Date = ");
  if (g_current_date.Day < 10) Serial.print("0");
  Serial.print(g_current_date.Day);
  Serial.print("-");
  if (g_current_date.Month < 10) Serial.print("0");
  Serial.print(g_current_date.Month);
  Serial.print("-");
  Serial.print(g_current_date.Year + 2000); // Convert back to full year

  Serial.print(" | Unix Time = ");
  Serial.println(g_current_unixtime);
}
void DoorDisplay()
{
  Serial.print("Door = ");
  Serial.print(Door_GetState());
  return;
}



//void testKeyWithBuzzer(void) {
//  uint8 key = KEYPAD_getPressedNewKey();
//  if (key != 255) {
//    for (uint8 i = 0; i < key; i++) {
//      digitalWrite(6, HIGH);
//      delay(100);
//      digitalWrite(6, LOW);
//      delay(200);
//    }
//  }
//  return;
//}
//void testRTCwithBuzzer(void) {
//
//  for (uint8 i = 0; i < g_current_date.Minute; i++) {
//    digitalWrite(6, HIGH);
//    delay(200);
//    digitalWrite(6, LOW);
//    delay(200);
//  }
//  delay(2000);
//  for (uint8 i = 0; i < g_current_date.Hour; i++) {
//    digitalWrite(6, HIGH);
//    delay(200);
//    digitalWrite(6, LOW);
//    delay(200);
//  }
//  delay(2000);
//  for (uint8 i = 0; i < g_current_date.Day; i++) {
//    digitalWrite(6, HIGH);
//    delay(200);
//    digitalWrite(6, LOW);
//    delay(200);
//  }
//  delay(2000);
//  for (uint8 i = 0; i < g_current_date.Month; i++) {
//    digitalWrite(6, HIGH);
//    delay(200);
//    digitalWrite(6, LOW);
//    delay(200);
//  }
//  delay(2000);
//  for (uint8 i = 0; i < g_current_date.Year; i++) {
//    digitalWrite(6, HIGH);
//    delay(200);
//    digitalWrite(6, LOW);
//    delay(200);
//  }
//  delay(2000);
//}
//
//void testValidationWithBuzzer(void) {
//
//  for (uint8 i = 0; i < g_user_state + 1; i++) {
//    digitalWrite(6, HIGH);
//    delay(100);
//    digitalWrite(6, LOW);
//    delay(200);
//  }
//  delay(2000);
//  return;
//}
//void testRoomWithBuzzer(void) {
//
//
//  for (uint8 i = 0; i < g_unit_location.Room; i++) {
//    digitalWrite(6, HIGH);
//    delay(200);
//    digitalWrite(6, LOW);
//    delay(200);
//  }
//  delay(2000);
//  for (uint8 i = 0; i < g_unit_location.Floor; i++) {
//    digitalWrite(6, HIGH);
//    delay(200);
//    digitalWrite(6, LOW);
//    delay(200);
//  }
//  delay(2000);
//  for (uint8 i = 0; i < g_unit_location.Building; i++) {
//    digitalWrite(6, HIGH);
//    delay(200);
//    digitalWrite(6, LOW);
//    delay(200);
//  }
//  delay(2000);
//}
