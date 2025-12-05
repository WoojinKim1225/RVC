#include <stdbool.h>
#include "../include/rvc.h"

#define SON 127
#define SIDLE 64
#define SOFF 0
#define SREV -127

/* Stub for the wait function */
void rvc_wait(int ticks)
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
        clean = SOFF;
        break;
    case ON:
        clean = SIDLE;
        break;
    case UP:
        clean = SON;
        break;
    }
}

void Motor(MotorCommand cmd)
{
    switch (cmd)
    {
    case MOVE_FWD:
        left = SON;
        right = SON;
        break;
    case STOP:
        left = SOFF;
        right = SOFF;
        break;
    case MOVE_BACK:
        left = SREV;
        right = SREV;
        break;
    case TURN_LEFT:
        left = SREV;
        right = SON;
        break;
    case TURN_RIGHT:
        left = SREV;
        right = SON;
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
