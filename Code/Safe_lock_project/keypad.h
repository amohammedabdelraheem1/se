/******************************************************************************
*
* Module: KEYPAD
*
* File Name: keypad.h
*
* Description: Header file for the Keypad driver
*
* Author: Ahmed Mohammed Abdelraheem
*
*******************************************************************************/

#ifndef KEYPAD_H_
#define KEYPAD_H_
#include <Arduino.h>
// Preprocessor directive to ensure C++ compatibility
// Prevents name mangling when used in C++ projects
#ifdef __cplusplus
extern "C" {
#endif   //__cplusplus

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/* Keypad configurations for number of rows and columns */
#define KEYPAD_NUM_COLS                   3 // Number of columns in the keypad
#define KEYPAD_NUM_ROWS                   4 // Number of rows in the keypad
#define KEYPAD_DEBOUNCING_NUM             5 // Number of times to debounce the keypad
#define NONPRESSED                        0xFF // Value returned when no key is pressed             

/* Keypad pin Configurations */
#define KEYPAD_ROW1_PIN                   2  // PD2 (Pin 2)
#define KEYPAD_ROW2_PIN                   14 // PC0 (Pin 14)
#define KEYPAD_ROW3_PIN                   15 // PC1 (Pin 3)
#define KEYPAD_ROW4_PIN                   17 // A3 PC3 (Pin 17)
#define KEYPAD_COL1_PIN                   3 // PD3 (Pin 3)
#define KEYPAD_COL2_PIN                   0 // PD1 (Pin 1)---------------->
#define KEYPAD_COL3_PIN                   16 // PC2 (Pin 16)

/* Keypad button logic configurations */
#define KEYPAD_BUTTON_PRESSED            LOW // Logic level when a button is pressed
#define KEYPAD_BUTTON_RELEASED           HIGH // Logic level when a button is released

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Get the Keypad pressed button
 */
uint8_t KEYPAD_getPressedKey(void); // Function prototype for getting the pressed key
uint8_t KEYPAD_getPressedNewKey(void);
#ifdef __cplusplus
}
#endif   //__cplusplus
#endif /* KEYPAD_H_ */
