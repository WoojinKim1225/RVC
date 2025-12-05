#include <stdbool.h>
#include "../include/rvc.h"

#define ON 127
#define IDLE 64
#define OFF 0
#define REV -127

/* Stub for the wait function */
void wait(int ticks)
{
    (void)ticks; // To avoid unused variable warning
}

MotorCommand lastMotorCommand;
CleanerCommand lastCleanerCommand;

int clean;
int left;
int right;

/*
 * Stub implementations for cleaner control functions.
 * During tests, we don't control real hardware, so these provide
 * empty implementations or default return values to allow linking.
 */

void Cleaner(CleanerCommand com)
{
    lastCleanerCommand = com;
    switch (com)
    {
    case OFF:
        clean = OFF;
        break;
    case ON:
        clean = IDLE;
        break;
    case UP:
        clean = ON;
        break;
    }
}

void Motor(MotorCommand cmd)
{
    switch (cmd)
    {
    case MOVE_FWD:
        left = ON;
        right = ON;
        break;
    case STOP:
        left = OFF;
        right = OFF;
        break;
    case MOVE_BACK:
        left = REV;
        right = REV;
        break;
    case TURN_LEFT:
        left = REV;
        right = ON;
        break;
    case TURN_RIGHT:
        left = REV;
        right = ON;
        break;
    }
}

/*
 * Stub implementations for actuator control functions.
 * These stubs record the command in a global variable for test verification.
 */
MotorCommand MoveForward(bool enable)
{
    lastMotorCommand = enable ? MOVE_FWD : STOP;
    Motor(lastMotorCommand);
    return lastMotorCommand;
}

MotorCommand MoveBackward(bool enable)
{
    lastMotorCommand = enable ? MOVE_BACK : STOP;
    Motor(lastMotorCommand);
    return lastMotorCommand;
}

MotorCommand TurnLeft()
{
    lastMotorCommand = TURN_LEFT;
    Motor(lastMotorCommand);
    return lastMotorCommand;
}

MotorCommand TurnRight()
{
    lastMotorCommand = TURN_RIGHT;
    Motor(lastMotorCommand);
    return lastMotorCommand;
}
