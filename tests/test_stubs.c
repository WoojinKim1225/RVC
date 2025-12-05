#include <stdbool.h>
#include "../include/rvc.h"

/* Stub for the wait function */
void wait(int ticks)
{
    (void)ticks; // To avoid unused variable warning
}

MotorCommand lastMotorCommand;
CleanerCommand lastCleanerCommand;

/*
 * Stub implementations for cleaner control functions.
 * During tests, we don't control real hardware, so these provide
 * empty implementations or default return values to allow linking.
 */

void Cleaner(CleanerCommand com) { lastCleanerCommand = com; }

/*
 * Stub implementations for actuator control functions.
 * These stubs record the command in a global variable for test verification.
 */
MotorCommand MoveForward(bool enable)
{
    lastMotorCommand = enable ? MOVE_FWD : STOP;
    return lastMotorCommand;
}

MotorCommand MoveBackward(bool enable)
{
    lastMotorCommand = enable ? MOVE_BACK : STOP;
    return lastMotorCommand;
}

MotorCommand TurnLeft()
{
    return lastMotorCommand = TURN_LEFT;
}

MotorCommand TurnRight()
{
    return lastMotorCommand = TURN_RIGHT;
}