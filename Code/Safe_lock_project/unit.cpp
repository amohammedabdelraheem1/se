#include "unit.h"
#include <EEPROM.h>
/************************Variables definetions**********************************/
static tLocation g_unit_location;
void UNIT_Init(void)
{
    g_unit_location.Room = EEPROM.read(EEPROM_ROOM_BYTE_NUM);
    g_unit_location.Floor = EEPROM.read(EEPROM_FLOOR_BYTE_NUM);
    g_unit_location.Building = EEPROM.read(EEPROM_BUILDING_BYTE_NUM);
}
void UNIT_SetLocation(uint8_t room, uint8_t floor, uint8_t building)
{
    g_unit_location.Room = room;
    g_unit_location.Floor = floor;
    g_unit_location.Building = building;
    EEPROM.write(EEPROM_ROOM_BYTE_NUM, g_unit_location.Room);
    EEPROM.write(EEPROM_FLOOR_BYTE_NUM, g_unit_location.Floor);
    EEPROM.write(EEPROM_BUILDING_BYTE_NUM, g_unit_location.Building);
}
tLocation UNIT_GetLocation(void)
{
    return g_unit_location;
}
bool UNIT_IsSameLocation(tLocation location)
{
    return (g_unit_location.Room == location.Room &&
            g_unit_location.Floor == location.Floor &&
            g_unit_location.Building == location.Building);
}