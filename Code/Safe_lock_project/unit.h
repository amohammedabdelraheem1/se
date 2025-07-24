#ifndef UNIT_H
#define UNIT_H
#include "Arduino.h"
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
/**************************************************************/
#define EEPROM_ROOM_BYTE_NUM 8
#define EEPROM_FLOOR_BYTE_NUM 9
#define EEPROM_BUILDING_BYTE_NUM 10
/**************************************************************
/************************Variables definetions**********************************/
typedef struct
{
  uint8_t Room;
  uint8_t Floor;
  uint8_t Building;
}tLocation; // UnitLocation

/************************Function prototypes**********************************/
void UNIT_Init(void);
void UNIT_SetLocation(uint8_t room, uint8_t floor, uint8_t building);
tLocation UNIT_GetLocation(void);
bool UNIT_IsSameLocation(tLocation location);

/************************End of function prototypes****************************/
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // UNIT_H
