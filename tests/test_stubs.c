#include <stdbool.h>
#include "../include/rvc.h"

/*
 * Stub variables for sensor inputs, controlled by test cases.
 * These are referenced via 'extern' in test_wjk.cpp.
 */
bool stub_front_input = false;
int stub_left_input = 0;
int stub_right_input = 0;
int stub_dust_input = 0;

/*
 * Stub implementations for hardware-dependent sensor reading functions.
 * They return the values of the stub variables.
 */
bool ReadFrontSensor(void) { return stub_front_input; }
int ReadLeftSensor(void) { return stub_left_input; }
int ReadRightSensor(void) { return stub_right_input; }
int ReadDustSensor(void) { return stub_dust_input; }

/* Stub for the wait function from 2.c */
void wait(int ticks)
{
    (void)ticks; // To avoid unused variable warning
}

/*
 * Stub implementations for actuator control functions.
 * During tests, we don't control real hardware, so these provide
 * empty implementations or default return values to allow linking.
 */
MotorCommand MoveForward(bool enable)
{
    (void)enable;
    return MOVE_FWD;
}
MotorCommand MoveBackward(bool enable)
{
    (void)enable;
    return MOVE_BACK;
}
MotorCommand TurnLeft()
{
    return TURN_LEFT;
}
MotorCommand TurnRight()
{
    return TURN_RIGHT;
}
void Cleaner(CleanerCommand com) { (void)com; }