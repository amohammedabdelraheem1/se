#ifndef UNIT_H
#define UNIT_H
#include <Arduino.h>
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "common_types.h"
/**************************************************************/
#define EEPROM_ROOM_BYTE_NUM 8
#define EEPROM_FLOOR_BYTE_NUM 9
#define EEPROM_BUILDING_BYTE_NUM 10
/**************************************************************
/************************Variables definetions**********************************/

/************************Function prototypes**********************************/
void UNIT_Init(void);
void UNIT_SetLocation(uint8_t room, uint8_t floor, uint8_t building);
tLocation UNIT_GetLocation(void);
bool UNIT_IsSameLocation(tLocation* ptr2location);

/************************End of function prototypes****************************/
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // UNIT_H
