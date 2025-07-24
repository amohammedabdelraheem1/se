
#include "password.h"
#include "keypad.h"
#include "buzzer.h"
#include "door.h"
#include "dc_motor.h"
#include"rfid.h"
#include "mode.h"
#include "rtc.h"
#include "unit.h"

/*************************************************************/
#define PROGRAMMIN_PERIOD 60000  // mill second
/*************************************************************/

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
//8888888888888888(R, F, B)
  UNIT_SetLocation(5, 1, 2);
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

/**************************testing function*******************************/






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
