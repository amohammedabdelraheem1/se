#ifndef LOCK_H_
#define LOCK_H_

// Preprocessor directive to ensure C++ compatibility
// Prevents name mangling when used in C++ projects
#ifdef __cplusplus
extern "C" {
#endif   //__cplusplus

//************************    Pins connection    *****************************/
// Pin definitions for limit switches 
// Indicates the microcontroller pins used for detecting lock's open and closed positions
#define CLOSE_LIMIT_PIN   5 //PD5 - Pin for close limit switch
#define OPEN_LIMIT_PIN    8 //PB0 - Pin for open limit switch

/************************ Lock State Enum *****************************/
// Defines possible states for the lock
// Provides a clear representation of the lock's current position
typedef enum {
    UNKNOWN,  // Initial state or when position cannot be determined
    OPEN,     // Lock is in fully open position
    CLOSED    // Lock is in fully closed position
} tLock_state;

/************************ Global Variables *****************************/
// Static variable to track current lock state
// Initialized to UNKNOWN to represent initial undefined state
static tLock_state s_lock_state = UNKNOWN;

/************************ Functions Declaration *****************************/
// Function to initialize lock system and associated hardware
extern void Lock_Init(void);

// Periodic function to update lock state and handle state transitions
extern void Lock_Update(void);

// Retrieve current lock state
extern tLock_state Lock_GetState(void);

// Interrupt Service Routine for open limit switch
// Triggered when open limit switch is activated
extern void Lock_openLimit_ISR(void);

// Interrupt Service Routine for close limit switch
// Triggered when close limit switch is activated
extern void Lock_closeLimit_ISR(void);

// Preprocessor directive to close C++ compatibility block
#ifdef __cplusplus
}
#endif   //__cplusplus

// Header guard to prevent multiple inclusions
#endif  //LOCK_H_
