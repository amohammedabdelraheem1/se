/******************************************************************************

  Module: KEYPAD

  File Name: keypad.c

  Description: Source file for the Keypad driver

  Author: Ahmed Mohammed Abdelraheem

*******************************************************************************/

#include "keypad.h"   // Include the keypad header file
#include <arduino.h>  // Include the Arduino library
#include "buzzer.h"

/*******************************************************************************
                        Functions Prototypes(Private)
 *******************************************************************************/

static const uint8 Arr_rows[] = { KEYPAD_ROW1_PIN, KEYPAD_ROW2_PIN, KEYPAD_ROW3_PIN, KEYPAD_ROW4_PIN };  // Array of row pins
static const uint8 Arr_cols[] = { KEYPAD_COL1_PIN, KEYPAD_COL2_PIN, KEYPAD_COL3_PIN };                   // Array of column pins

static uint8 KEYPAD_4x3_adjustKeyNumber(uint8 button_number);  // Function to adjust the key number for a 4x3 keypad
uint8 KEYPAD_getPressedKey(void);
uint8 KEYPAD_getPressedNewKey(void);
/*******************************************************************************
                        Functions Definitions
 *******************************************************************************/

/**
   @brief Get the Pressed Key on the Keypad
   @return The pressed key value, or NONPRESSED if no key is pressed
*/
uint8 KEYPAD_getPressedKey(void) {
  // Set all row pins as input with pull-up resistors
  pinMode(KEYPAD_ROW1_PIN, INPUT_PULLUP);
  pinMode(KEYPAD_ROW2_PIN, INPUT_PULLUP);
  pinMode(KEYPAD_ROW3_PIN, INPUT_PULLUP);
  pinMode(KEYPAD_ROW4_PIN, INPUT_PULLUP);

  // Set all column pins as input with pull-up resistors
  pinMode(KEYPAD_COL1_PIN, INPUT_PULLUP);
  pinMode(KEYPAD_COL2_PIN, INPUT_PULLUP);
  pinMode(KEYPAD_COL3_PIN, INPUT_PULLUP);

  // Iterate through the rows
  for (uint8 i = 0; i < KEYPAD_NUM_ROWS; i++) {
    // Set the current row pin as output and drive it low
    pinMode(Arr_rows[i], OUTPUT);
    digitalWrite(Arr_rows[i], LOW);

    // Check the column pins for a pressed button
    for (uint8 j = 0; j < KEYPAD_NUM_COLS; j++) {
      if (digitalRead(Arr_cols[j]) == KEYPAD_BUTTON_PRESSED) {
        // Debounce the button press
        for (uint8 w = 0; w < KEYPAD_DEBOUNCING_NUM; w++) {
          if (digitalRead(Arr_cols[j]) == KEYPAD_BUTTON_RELEASED) {
            return NONPRESSED;
          }
        }
        // Return the adjusted key number
        return KEYPAD_4x3_adjustKeyNumber(((i * KEYPAD_NUM_COLS) + j + 1));
      }
    }

    // Set the current row pin back to input with pull-up
    pinMode(Arr_rows[i], INPUT_PULLUP);
  }

  // No key is pressed
  return NONPRESSED;
}

/**
   @brief Adjust the key number for a 4x3 keypad
   @param button_number The raw button number (1-12)
   @return The adjusted key value
*/
static uint8 KEYPAD_4x3_adjustKeyNumber(uint8 button_number) {
  uint8 keypad_button = 0;
  switch (button_number) {
    case 10:
      keypad_button = '*';  // ASCII code for '*'
      break;
    case 11:
      keypad_button = 0;
      break;
    case 12:
      keypad_button = '#';  // ASCII code for '#'
      break;
    default:
      keypad_button = button_number;
      break;
  }
  return keypad_button;
}
uint8 KEYPAD_getPressedNewKey() {
  static uint8 sl_curretKey = NONPRESSED, sl_prevKey = NONPRESSED, sl_newKeyFlag = 0;
  static uint16 sl_debouncingCounter = 0;
  sl_curretKey = KEYPAD_getPressedKey();
  if (sl_curretKey == sl_prevKey) {
    if (sl_debouncingCounter <= DEBOUNCING_TIMES) {
      sl_debouncingCounter++;
    }
    if (sl_debouncingCounter >= DEBOUNCING_TIMES && sl_newKeyFlag == 0) {
      sl_newKeyFlag = 1;
      if (sl_curretKey != NONPRESSED) {
        Buzzer_setTime(NEWKEYPRESSED_BUZZER);
      }
      return sl_curretKey;
    }
  } else {
    sl_debouncingCounter = 0;
    sl_newKeyFlag = 0;
    sl_prevKey = sl_curretKey;
  }

  return NONPRESSED;
}
