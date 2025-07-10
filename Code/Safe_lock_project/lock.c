#include "lock.h"
#include "std_types.h"
#include "dc_motor.h"
#include <arduino.h>

// Initialize lock system and interrupt configuration
void Lock_Init()
{
    // Configure interrupts for open and close limit switches
    // Using falling edge (transition from HIGH to LOW) to trigger interrupts
    attachInterrupt(digitalPinToInterrupt(OPEN_LIMIT_PIN), Lock_openLimit_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(CLOSE_LIMIT_PIN), Lock_closeLimit_ISR, FALLING);
    
    // Set initial lock state to unknown
    s_lock_state = UNKNOWN;
    return;
}

// Periodically update lock state based on limit switch readings
void Lock_Update()
{
    // Read current state of limit switches
    uint8 l_closeLimitState = digitalRead(CLOSE_LIMIT_PIN);
    uint8 l_openLimitState  = digitalRead(OPEN_LIMIT_PIN);

    // Anomalous state: both switches activated or deactivated
    if((l_openLimitState == LOGIC_LOW && l_closeLimitState == LOGIC_LOW) || 
       (l_openLimitState == LOGIC_HIGH && l_closeLimitState == LOGIC_HIGH))
    {
        s_lock_state = UNKNOWN;
        return;
    }
    
    // Open state: open switch activated, close switch not activated
    else if(l_openLimitState == LOGIC_LOW && l_closeLimitState == LOGIC_HIGH)
    {
        s_lock_state = OPEN;
        return;
    }
    
    // Closed state: close switch activated, open switch not activated
    else if(l_openLimitState == LOGIC_HIGH && l_closeLimitState == LOGIC_LOW)
    {
        s_lock_state = CLOSED;
        return;
    }
    return;
}

// Return current lock state
tLock_state Lock_GetState()
{
    return s_lock_state;
}

// Interrupt Service Routine for open limit switch
void Lock_openLimit_ISR()
{
    // Stop motor when open limit switch is triggered
    if(digitalRead(OPEN_LIMIT_PIN) == LOW)
    {
        DC_Motor_turnOff();
    }
    return;
}

// Interrupt Service Routine for close limit switch
void Lock_closeLimit_ISR()
{
    // Stop motor when close limit switch is triggered
    if(digitalRead(CLOSE_LIMIT_PIN) == LOW)
    {
        DC_Motor_turnOff();
    }
    return;
}
