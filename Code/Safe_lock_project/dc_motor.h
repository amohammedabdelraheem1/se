#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

// Preprocessor directive to ensure C++ compatibility
// Prevents name mangling when used in C++ projects
#ifdef __cplusplus
extern "C" {
#endif   //__cplusplus

/************************    Pins connection    *****************************/
// Pin definition for DC motor control
// Specifies the microcontroller pin used to drive the motor
#define DC_MOTOR_PIN      7  //PD7 - Pin for motor control

/************************ Functions Declaration *****************************/
// Initialize motor control pin and related hardware
extern void DC_MOTOR_Init(void);

// Turn on the DC motor 
// Typically sets the control pin to enable motor operation
extern void DC_MOTOR_TurnOn(void);

// Turn off the DC motor
// Typically sets the control pin to disable motor operation
extern void DC_MOTOR_TurnOff(void);

// Preprocessor directive to close C++ compatibility block
#ifdef __cplusplus
}
#endif   //__cplusplus

// Header guard to prevent multiple inclusions
#endif  //DC_MOTOR_H_
