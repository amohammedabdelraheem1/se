// Header guard to prevent multiple inclusions
#ifndef DOOR_H_
#define DOOR_H_

// Extern "C" block to ensure compatibility with C++ compilation
#ifdef __cplusplus
extern "C" {
#endif

// Pin definition for proximity sensor
#define PROXIMITY_PIN     5   //PD5 (Pin D5)
#define DUBOUNCING_COUNTER  2
// Enumeration to represent door states
typedef enum {
  OPEN,   // Door is open
  CLOSED  // Door is closed
} tDoor_state;

// Static variable to track current door state
// Note: There's a typo in the type (tLDoor_state instead of tDoor_state)
static tDoor_state s_door_state = OPEN;

// Function declarations for door control
extern void Door_Init(void);       // Initialize door system
extern void Door_Update(void);     // Update door state
extern tDoor_state Door_GetState(void);  // Retrieve current door state

// Close extern "C" block for C++ compatibility
#ifdef __cplusplus
}
#endif

#endif  //DOOR_H_
