#include "rfid.h"
#include "rtc.h"
#include "unit.h"

/************************Variables definetions**********************************/
tUserState g_user_state = INVALID_STATE;
tCard g_Current_card, g_Prev_card;
uint8_t g_buffer[18], g_prev_buffer[18], g_buffer_size = sizeof(g_buffer);
tDate  g_sync_date;

uint32_t g_current_unixtime, g_expired_unixtime;
/*******************************************************************************/
void RFID_Init(void) {
  while (!Serial)
    ;                  // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (uint8_t i = 0; i < 6; i++) {
    key.keyuint8_t[i] = 0xFF;
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
  g_Current_card.ID = g_buffer[ID_uint8_t_NUM];
  g_Current_card.ExpirdDate.Minute = (g_buffer[MINET_uint8_t_NUM] & 0x7f);
  g_Current_card.ExpirdDate.Hour = (((g_buffer[MINET_uint8_t_NUM] & 0x80) >> 3) | (g_buffer[HOUR_uint8_t_NUM] >> 4));
  g_Current_card.ExpirdDate.Day = (g_buffer[MONTH_DAY_uint8_t_NUM] & 0x0f) | ((g_buffer[YEAR_uint8_t_NUM] & 0x80) >> 3);
  g_Current_card.ExpirdDate.Month = (g_buffer[MONTH_DAY_uint8_t_NUM] >> 4);
  g_Current_card.ExpirdDate.Year = (g_buffer[YEAR_uint8_t_NUM] & 0x7f);
  g_Current_card.CardLocation.Room = g_buffer[ROOM_uint8_t_NUM];
  g_Current_card.CardLocation.Floor = g_buffer[FLOOR_uint8_t_NUM];
  g_Current_card.CardLocation.Building = g_buffer[BUILDING_uint8_t_NUM];
  g_Current_card.CRC = g_buffer[CRC_uint8_t_NUM];
  g_expired_unixtime = UnixTime(g_Current_card.ExpirdDate);
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

    if (RTC_IsTimeValid(g_Current_card.ExpirdDate)) // check the date
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
        if (UNIT_IsSameLocation(g_Current_card.CardLocation))
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
    // Serial.println("isNewCardPresent");
    // Serial.println(s_NoCardCounter);
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    // Serial.println("ReadCardSerial");
    return;
  }
  s_NoCardCounter = 0;
  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F("Card UID:"));
  // dump_uint8_t_array(mfrc522.uid.uiduint8_t, mfrc522.uid.size);
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
