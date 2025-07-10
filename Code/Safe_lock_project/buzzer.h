#ifndef BUZZER_H_
#define BUZZER_H_

// Preprocessor directive to ensure C++ compatibility
// Prevents name mangling when used in C++ projects
#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

//************************    Pins connection    *****************************/
#define BUZZER_PIN 6  //PD6 - Pin for BUZZER
#define NEWKEYPRESSED_DELAY 100
#define INVALID_PASS_DELAY 100  //DOUBLE
#define VALID_PASS_DELAY 500
#define INVALID_COMPARE_DELAY 100  //DOUBLE
#define VALID_COMPARE_DELAY 500
#define COMFIRMRD_DELAY 500


  /************************ Lock State Enum *****************************/
  // Defines possible states for the lock
  // Provides a clear representation of the lock's current position
  typedef enum {
    OFF_STATE,
    NEWKEYPRESSED_BUZZER,
    INVALID_PASS_BUZZER,
    VALID_PASS_BUZZER,
    INVALID_COMPARE_BUZZER,
    VALID_COMPARE_BUZZER,
    COMFIRMRD_BUZZER
  } tBuzzer_state;

  /************************ Global Variables *****************************/
  extern tBuzzer_state g_buzzerState;

  /************************ Functions Declaration *****************************/
  // Function to initialize BUZZER system and associated hardware
  void Buzzer_Init(void);

  // Periodic function to update buzzer state and handle state transitions
  void Buzzer_Update(void);

  void Buzzer_setTime(tBuzzer_state l_buzzerState);


// Preprocessor directive to close C++ compatibility block
#ifdef __cplusplus
}
#endif  //__cplusplus

// Header guard to prevent multiple inclusions
#endif  //BUZZER_H_
