// Header guard to prevent multiple inclusions
#ifndef DOOR_H_
#define DOOR_H_

// Extern "C" block to ensure compatibility with C++ compilation
#ifdef __cplusplus
extern "C" {
#endif
#define MOTOR_MAX_ON_TIME_MS 2000
//************************    Pins connection    *****************************/
// Pin definitions for limit switches 
// Indicates the microcontroller pins used for detecting lock's open and closed positions
#define CLOSE_LIMIT_PIN   5 //PD5 - Pin for close limit switch
#define OPEN_LIMIT_PIN    8 //PB0 - Pin for open limit switch

// Pin definition for proximity sensor
#define PROXIMITY_PIN     4   //PD4 (Pin D4)
#define DUBOUNCING_COUNTER  2

// Defines possible states for the lock
// Provides a clear representation of the lock's current position
typedef enum {
    LOCK_UNKNOWN,  // Initial state or when position cannot be determined
    LOCK_OPEN,     // Lock is in fully open position
    LOCK_CLOSED    // Lock is in fully closed position
} tLock_state;

// Enumeration to represent door states
typedef enum {
  DOOR_OPEN,   // Door is open
   DOOR_CLOSED  // Door is closed
} tDoor_state;

// Static variable to track current door state
// Note: There's a typo in the type (tLDoor_state instead of tDoor_state)
static tDoor_state s_door_state = DOOR_OPEN;

// Function declarations for door control
void Door_Init(void);       // Initialize door system
void Door_Update(void);     // Update door state
tDoor_state Door_GetState(void);  // Retrieve current door state
void Door_Lock(void);
void Door_Unlock(void);


// Close extern "C" block for C++ compatibility
#ifdef __cplusplus
}
#endif

#endif  //DOOR_H_
