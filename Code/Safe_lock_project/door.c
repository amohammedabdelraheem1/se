#include"std_types.h"
#include"door.h"
#include "dc_motor.h"
#include "lock.h"
#include<arduino.h>

static tLock_state s_lock_state = LOCK_UNKNOWN;
uint8 l_closeLimitState ;
uint8 l_openLimitState ;
void Door_Init()
{ pinMode(OPEN_LIMIT_PIN, INPUT_PULLUP);
  pinMode(CLOSE_LIMIT_PIN, INPUT_PULLUP);
  pinMode(PROXIMITY_PIN, INPUT_PULLUP);
  s_door_state = DOOR_OPEN;
  return;
}
void Door_Update()
{
  static uint8 sl_closedCounter = 0;
  static uint8 sl_openCounter = 0;
  if (digitalRead(PROXIMITY_PIN) == LOW)
  {
    sl_openCounter = 0;
    if (sl_closedCounter  != DUBOUNCING_COUNTER)
    {
      sl_closedCounter++;
    }

    if (sl_closedCounter >= DUBOUNCING_COUNTER )
    {
      s_door_state = DOOR_CLOSED;
      delay(500);
      Door_Lock();
      
    }

  }
  else
  {

    sl_closedCounter = 0;
    if (sl_openCounter  != DUBOUNCING_COUNTER)
    {
      sl_openCounter++;
    }

    if (sl_openCounter >= DUBOUNCING_COUNTER)
    {
      s_door_state = DOOR_OPEN;
    }
  }
  return;

}
tDoor_state Door_GetState()
{
  return s_door_state;
}
void Door_Lock(void)
{
  unsigned long motorStartTime = millis();

  if (s_door_state == DOOR_CLOSED && (digitalRead(CLOSE_LIMIT_PIN) == HIGH))
  {
    do {
      DC_MOTOR_TurnOn();
    } while ((digitalRead(CLOSE_LIMIT_PIN) == HIGH) &&(millis() - motorStartTime)< MOTOR_MAX_ON_TIME_MS);
    DC_MOTOR_TurnOff();
  }
  return ;
}
void Door_Unlock(void)
{
  unsigned long motorStartTime = millis();

  if (s_door_state == DOOR_OPEN && (digitalRead(OPEN_LIMIT_PIN) == HIGH))
  {
    do {
      DC_MOTOR_TurnOn();
    } while ((digitalRead(OPEN_LIMIT_PIN) == HIGH) &&(millis() - motorStartTime)< MOTOR_MAX_ON_TIME_MS);
    DC_MOTOR_TurnOff();
  }
  return ;
}
