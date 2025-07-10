#include"std_types.h"
#include"door.h"
#include<arduino.h>


void Door_Init()
{
    pinMode(PROXIMITY_PIN,INPUT_PULLUP);
    s_door_state = OPEN;
    return;
}
void Door_Update()
{
    static uint8 sl_closedCounter = 0;
    static uint8 sl_openCounter = 0;
    if(digitalRead(PROXIMITY_PIN) == LOW)
    {
        sl_openCounter = 0;
        if(sl_closedCounter  != DUBOUNCING_COUNTER)
        {
            sl_closedCounter++;
        }

        if(sl_closedCounter >= DUBOUNCING_COUNTER)
        {
            s_door_state = CLOSED;
        }

    }
    else
    {

        sl_closedCounter = 0;
        if(sl_openCounter  != DUBOUNCING_COUNTER)
        {
            sl_openCounter++;
        }

        if(sl_openCounter >= DUBOUNCING_COUNTER)
        {
            s_door_state = OPEN;
        }
    }
    return;

}
 tDoor_state Door_GetState()
 {
     
     return s_door_state;
 }
