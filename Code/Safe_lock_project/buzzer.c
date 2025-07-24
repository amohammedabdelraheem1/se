#include "buzzer.h"
#include <Arduino.h>  

tBuzzer_state g_buzzerState = OFF_STATE;
uint32_t g_startBuzzerTime = 0;
void Buzzer_Init(void) {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  return;
}
void Buzzer_setTime(tBuzzer_state l_buzzerState) {
  g_buzzerState = l_buzzerState;
  if (l_buzzerState != OFF_STATE) {
    g_startBuzzerTime = millis();
    digitalWrite(BUZZER_PIN, HIGH);
  }
  return;
}
void Buzzer_Update(void) {
  static uint8_t invalidBassFlag = 0;
  static uint8_t invalidCompFlag = 0;
  switch (g_buzzerState) {
    case OFF_STATE:
      digitalWrite(BUZZER_PIN, LOW);
      break;
    case NEWKEYPRESSED_BUZZER:
      if ((millis() - g_startBuzzerTime) >= NEWKEYPRESSED_DELAY) {
        g_buzzerState = OFF_STATE;
        digitalWrite(BUZZER_PIN, LOW);
      }
      break;
    case INVALID_PASS_BUZZER:
      if (invalidBassFlag == 0) {
        if ((millis() - g_startBuzzerTime) >= INVALID_PASS_DELAY) {
          digitalWrite(BUZZER_PIN, LOW);
          g_startBuzzerTime = millis();
          invalidBassFlag = 1;
        }
      } else if (invalidBassFlag == 1) {
        if ((millis() - g_startBuzzerTime) >= (INVALID_PASS_DELAY / 2)) {
          digitalWrite(BUZZER_PIN, HIGH);
          g_startBuzzerTime = millis();
          invalidBassFlag = 2;
        }

      } else if (invalidBassFlag == 2) {
        if ((millis() - g_startBuzzerTime) >= INVALID_PASS_DELAY) {
          digitalWrite(BUZZER_PIN, LOW);
          g_buzzerState = OFF_STATE;
          invalidBassFlag = 0;
        }
      }
      break;
    case VALID_PASS_BUZZER:
      if ((millis() - g_startBuzzerTime) >= VALID_PASS_DELAY) {
        g_buzzerState = OFF_STATE;
        digitalWrite(BUZZER_PIN, LOW);
      }
      break;
    case INVALID_COMPARE_BUZZER:
      if (invalidCompFlag == 0) {
        if ((millis() - g_startBuzzerTime) >= INVALID_COMPARE_DELAY) {
          digitalWrite(BUZZER_PIN, LOW);
          g_startBuzzerTime = millis();
          invalidCompFlag = 1;
        }
      } else if (invalidCompFlag == 1) {
        if ((millis() - g_startBuzzerTime) >= (INVALID_COMPARE_DELAY / 2)) {
          digitalWrite(BUZZER_PIN, HIGH);
          g_startBuzzerTime = millis();
          invalidCompFlag = 2;
        }
      } else if (invalidCompFlag == 2) {
        if ((millis() - g_startBuzzerTime) >= INVALID_COMPARE_DELAY) {
          digitalWrite(BUZZER_PIN, LOW);
          g_buzzerState = OFF_STATE;
          invalidCompFlag = 0;
        }
      }
      break;
    case VALID_COMPARE_BUZZER:
      if ((millis() - g_startBuzzerTime) >= VALID_COMPARE_DELAY) {
        g_buzzerState = OFF_STATE;
        digitalWrite(BUZZER_PIN, LOW);
      }
      break;
    case COMFIRMRD_BUZZER:
      if ((millis() - g_startBuzzerTime) >= COMFIRMRD_DELAY) {
        g_buzzerState = OFF_STATE;
        digitalWrite(BUZZER_PIN, LOW);
      }
      break;
  }
}
