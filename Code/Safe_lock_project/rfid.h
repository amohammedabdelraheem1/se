/******************************************************************************
 *
 * Module: RFID
 *
 * Description: Header file for the RFID driver
 *
 * Author: Ahmed Mohammed Abdelraheem
 *
 ******************************************************************************/

#ifndef RFID_H_
#define RFID_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <MFRC522.h>
#include "Arduino.h"
#include "common_types.h"
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
#define RST_PIN 9  // Configurable, see typical pin layout above
#define SS_PIN 10  // Configurable, see typical pin layout above
/**************************************************************/
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



/************************Variables definetions**********************************/
tUserState g_user_state = INVALID_STATE;
tCard g_Current_card, g_Prev_card;
tLocation g_unit_location;
byte g_buffer[18], g_prev_buffer[18], g_buffer_size = sizeof(g_buffer);
tDate g_current_date, g_sync_date;
unsigned long g_authorised_period, g_authorised_data, g_light_only_last_date, g_light_only_start_date, g_all_outputs_date;
unsigned long g_current_unixtime, g_expired_unixtime;
/*******************************************************************************/
void RFID_Init(void);
void CARD_Init(void);
void CardRead(void);
void CARD_Update(void);
void CHECK_VALIDATION_Init(void) ;
void CHECK_VALIDATION_Update(void);
tUserState CHECK_VALIDATION_GetState(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* RFID_H_ */
