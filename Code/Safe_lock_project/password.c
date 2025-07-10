#include "buzzer.h"
#include "password.h"
#include "keypad.h"
#include <arduino.h>
uint8 eg_savedPass[PASSWORD_SIZE];
uint8 eg_masterPass[PASSWORD_SIZE];
uint8 eg_userPass[PASSWORD_SIZE];

// Initialize passwords by reading from EEPROM

/*void Password_Init() {
  // Read saved client and master passwords from EEPROM
  for (uint8 i = 0; i < PASSWORD_SIZE; i++) {
    sg_savedPass[i] = EEPROM.read(EEPROM_CLIENT_PASS_FIRST_BYTR + i);
    eg_masterPass[i] = EEPROM.read(EEPROM_MASTER_PASS_FIRST_BYTR + i);
  }
  return;
}
*/

// Function to get password input from user
tPassword_state Password_getPass(uint8* ptrTopass) {

  uint8 l_tempPass[PASSWORD_SIZE];
  uint8 l_pressedKey = NONPRESSED;
  uint32 l_entryTime = millis();
  // Collect password digits
  for (uint8 i = 0; i < PASSWORD_SIZE; i++) {
    do {
      l_pressedKey = KEYPAD_getPressedNewKey();
       Buzzer_Update();
      // Check for timeout
      if ((millis() - l_entryTime) >= MAX_ENTRY_PASS_TIME) {
        Buzzer_setTime(INVALID_PASS_BUZZER);
        return INVAILD_PASSWORD;
      }
    } while (l_pressedKey == NONPRESSED);
    if (l_pressedKey == '*' || l_pressedKey == '#') {
      Buzzer_setTime(INVALID_PASS_BUZZER);
      return INVAILD_PASSWORD;
    }
    l_tempPass[i] = l_pressedKey;
  }

  // Wait for confirmation key
  do {
    l_pressedKey = KEYPAD_getPressedNewKey();
    Buzzer_Update();
    // Check for timeout
    if ((millis() - l_entryTime) >= MAX_ENTRY_PASS_TIME) {
      Buzzer_setTime(INVALID_PASS_BUZZER);
      return INVAILD_PASSWORD;
    }
  } while (l_pressedKey == NONPRESSED);

  // Check confirmation key
  if (l_pressedKey != '#') {
    Buzzer_setTime(INVALID_PASS_BUZZER);
    return INVAILD_PASSWORD;
  }

  // Copy temporary password to output
  for (uint8 i = 0; i < PASSWORD_SIZE; i++) {
    ptrTopass[i] = l_tempPass[i];
  }

  Buzzer_setTime(VALID_PASS_BUZZER);
  return VAILD_PASSWORD;
}

// Update password in EEPROM
/*
void Password_Update(uint8* ptrTopass, uint8 l_eeprom_first_byte) {
  // Write password to specified EEPROM location
  for (uint8 i = 0; i < PASSWORD_SIZE; i++) {
    // EEPROM.write(l_eeprom_first_byte + i, ptrTopass[i]);
  }
  return;
}
*/

// Set a new password
tPassword_state Password_setNew(uint8* ptrTopass) {
  uint8 l_tempPass1[PASSWORD_SIZE];
  uint8 l_tempPass2[PASSWORD_SIZE];
  // Get initial password input
  if (Password_getPass(l_tempPass1) == INVAILD_PASSWORD) {
    Buzzer_setTime(INVALID_PASS_BUZZER);
    return INVAILD_PASSWORD;
  }

  Buzzer_setTime(COMFIRMRD_BUZZER);

  // Get confirmation password
  if (Password_getPass(l_tempPass2) == INVAILD_PASSWORD) {
    Buzzer_setTime(INVALID_PASS_BUZZER);
    return INVAILD_PASSWORD;
  }

  // Compare passwords
  if (Password_compare(l_tempPass1, l_tempPass2) == INVAILD_COMPARE) {
    Buzzer_setTime(INVALID_PASS_BUZZER);
    return INVAILD_PASSWORD;
  }
  for (uint8 i = 0; i < PASSWORD_SIZE; i++) {
    ptrTopass[i] = l_tempPass1[i];
  }

Buzzer_setTime(VALID_PASS_BUZZER);
  return VAILD_PASSWORD;
}

// Compare two passwords
tCompare_state Password_compare(uint8* ptrTopass1, uint8* ptrTopass2) {
  // Check if passwords match
  for (uint8 i = 0; i < PASSWORD_SIZE; i++) {
    if (ptrTopass1[i] != ptrTopass2[i]) {
      return INVAILD_COMPARE;
    }
  }
  return VAILD_COMPARE;
}
