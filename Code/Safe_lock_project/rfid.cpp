#include "rfid.h"
#include "rtc.h"
#include "unit.h"
/*************************************************************/
#define RST_PIN 9  // Configurable, see typical pin layout above
#define SS_PIN 10  // Configurable, see typical pin layout above
/**************************************************************/
/************************RFID VARIABLES****************************/
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
MFRC522::MIFARE_Key key;
/***********************************************************/
/************************Variables definetions**********************************/
tUserState g_user_state = INVALID_STATE;
tCard g_Current_card, g_Prev_card;
uint8_t g_buffer[18], g_prev_buffer[18], g_buffer_size = sizeof(g_buffer);
tDate  g_sync_date;

/*******************************************************************************/
void RFID_Init(void) {
  while (!Serial)
    ;                  // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (uint8_t i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}
void CARD_Init(void)
{
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
void CARD_Update(void)
{
  uint8_t temp;
  /*****************read the  new card*********************/
  Serial.println("ReadCardHere");
  CardRead();
  /*****************Extract data***************************/
  temp = g_buffer[TYPE_BYTE_NUM];
  switch (temp)
  {
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
}
void CHECK_VALIDATION_Init(void)
{
  g_user_state = INVALID_STATE;
}
void CHECK_VALIDATION_Update(void)
{
  if (g_Current_card.Type == OTHER)
  {
    Serial.println("INVALID_STATE1");
    g_user_state = INVALID_STATE;
    return;
  }
  else
  {

    if (RTC_IsTimeValid(&(g_Current_card.ExpirdDate))) // check the date
    {
      Serial.println("date done");
      if ((g_Current_card.Type == AUTHORISED) || (g_Current_card.Type == ROOM) || (g_Current_card.Type == TIMESYNC))
      {
        Serial.println("AUTHORISED-ROOM_TIMESYNC done");
        g_user_state = VALID_STATE;
        return;
      }
      else if ((g_Current_card.Type == MASTER) || (g_Current_card.Type == EMRGANCY))
      {
        Serial.println("MASTER or EMRGANCY");
        g_user_state = VALID_STATE;
        return;
      }
      else if (g_Current_card.Type == CLIENT)
      {
        Serial.println("CLIENT");
        if (UNIT_IsSameLocation(&(g_Current_card.CardLocation))) // check the location
        {
          Serial.println("UNIT_IsSameLocation done");
          if (RTC_IsTimeValid(&(g_Current_card.ExpirdDate))) // check the time
          {
            Serial.println("Time done");
            g_user_state = VALID_STATE;
            return;
          }
          else
          {
            Serial.println("INVALID_STATE2");
            g_user_state = INVALID_STATE;
            return;
          }
        }
        else
        {
          Serial.println("Room, Floor, and Building done");
          g_user_state = VALID_STATE;
          return;
        }
      }
    }
  }

  Serial.println("INVALID_STATE");
  g_user_state = INVALID_STATE;
  return;
}
void CardRead(void)
{
  Serial.println("Starting read card");
  for (int i = 0; i < g_buffer_size; i++)
  {
    g_prev_buffer[i] = g_buffer[i];
  }
  static uint8_t s_NoCardCounter = 0;
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    s_NoCardCounter++;
    if (s_NoCardCounter > 5)
    {
      for (int i = 0; i < g_buffer_size; i++)
      {
        g_buffer[i] = 0xff;
      }

      s_NoCardCounter = 0;
    }
     Serial.println("isNewCardPresent");
     Serial.println(s_NoCardCounter);
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
     Serial.println("ReadCardSerial");
    return;
  }
  s_NoCardCounter = 0;
  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F("Card UID:"));
  // dump_BYTE_array(mfrc522.uid.uidBYTE, mfrc522.uid.size);
  Serial.println();
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Check for compatibility
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
  {
    Serial.println(F("This sample only works with MIFARE Classic cards."));
    return;
  }

  // In this sample we use the second sector,
  // that is: sector #1, covering block #4 up to and including block #7
  uint8_t sector = 4;
  uint8_t blockAddr = 16;
  uint8_t dataBlock[] = {
      0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
      0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
      0x09, 0x0a, 0xff, 0x0b, //  9, 10, 255, 11,
      0x0c, 0x0d, 0x0e, 0x0f  // 12, 13, 14, 15
  };
  uint8_t trailerBlock = 7;
  MFRC522::StatusCode status;

  // Authenticate using key A
  Serial.println(F("Authenticating using key A..."));
  status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
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
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    for (int i = 0; i < g_buffer_size; i++)
    {
      g_buffer[i] = g_prev_buffer[i];
    }
  }
  Serial.print(F("Data in block "));
  Serial.print(blockAddr);
  Serial.println(F(":"));
  Serial.println();
  mfrc522.PCD_StopCrypto1();
}
tUserState CHECK_VALIDATION_GetState(void)
{
  return g_user_state;
}
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
void CardDataDisplay(void) {
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
