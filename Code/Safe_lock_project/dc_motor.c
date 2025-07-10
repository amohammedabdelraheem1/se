#include "std_types.h"
#include "dc_motor.h"
#include <arduino.h>

// Initialize DC motor control pin
void DC_MOTOR_Init()
{
    // Configure motor control pin as output
    pinMode(DC_MOTOR_PIN, OUTPUT);
    
    // Ensure motor is initially turned off
    digitalWrite(DC_MOTOR_PIN, LOW);
    return;
}

// Turn on the DC motor by setting control pin HIGH
void DC_MOTOR_TurnOn()
{
    digitalWrite(DC_MOTOR_PIN, HIGH);
    return;
}

// Turn off the DC motor by setting control pin LOW
void DC_MOTOR_TurnOff()
{
    digitalWrite(DC_MOTOR_PIN, LOW);
    return;
}
