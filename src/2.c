#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/rvc.h"

#define TICK 10

int tickCount = 0;

typedef enum
{
    CLEANER_OFF,
    CLEANER_ON,
    CLEANER_UP
} CleanerState;

/* Function Prototypes */
WheelState WheelControl(WheelState wheel_state, SensorData obstacle_location, bool *cleaner_control_enable);
CleanerState CleanerControl(CleanerState cleaner_state, SensorData dust_existence, bool cleaner_control_enable);

/* This function is defined in test_stubs.c for testing purposes. */
void wait(int ticks);

/* Actuator interface functions from rvc.h that are used in this file */
MotorCommand MoveForward(bool enable);
MotorCommand MoveBackward(bool enable);
MotorCommand TurnLeft();
MotorCommand TurnRight();
void Cleaner(CleanerCommand com);

/* ------------------------------------------------------------------------- */
/* MAIN LOOP                                                                 */
/* ------------------------------------------------------------------------- */

#ifndef UNIT_TEST
int main(void)
{
    SensorData obstacle_location;
    SensorData dust_existence;

    bool cleaner_control_enable = false;
    WheelState wheel_state = W_STOP;
    CleanerState cleaner_state = CLEANER_OFF;

    while (1)
    {
        /* Step 1: Read logical sensor states */
        bool frontsensor_value = ReadFrontSensor();
        int leftsensor_value = ReadLeftSensor();
        int rightsensor_value = ReadRightSensor();
        int dustsensor_value = ReadDustSensor();

        bool F = FrontSensorInterface(frontsensor_value);
        bool L = LeftSensorInterface(leftsensor_value);
        bool R = RightSensorInterface(rightsensor_value);
        bool D = DustSensorInterface(dustsensor_value);

        obstacle_location = DetermineObstacleLocation(F, L, R);
        dust_existence = DetermineDustExistence(D);
        /* Step 2: Run MotorController*/
        wheel_state =
            WheelControl(wheel_state, obstacle_location,
                         &cleaner_control_enable);
        /* Step 3: Run Clean Controller*/
        cleaner_state =
            CleanerControl(cleaner_state, dust_existence,
                           cleaner_control_enable);
        /* Step 4: Wait for next control tick */
        wait(TICK);
    }
}
#endif /* UNIT_TEST */

/* ------------------------------------------------------------------------- */
/* SENSOR INTERFACES                                                         */
/* ------------------------------------------------------------------------- */

bool FrontSensorInterface(bool sensor_value)
{
    return sensor_value;
}

bool LeftSensorInterface(int analog_value)
{
    if (analog_value < 100)
    {
        return true;
    }
    return false;
}

bool RightSensorInterface(int analog_value)
{
    if (analog_value < 100)
    {
        return true;
    }
    return false;
}

bool DustSensorInterface(int analog_value)
{
    if (analog_value > 600)
    {
        return true;
    }
    return false;
}

/* ------------------------------------------------------------------------- */
/* DETERMINE SENSOR STATES                                                   */
/* ------------------------------------------------------------------------- */

SensorData
DetermineObstacleLocation(bool F, bool L, bool R)
{
    SensorData data = {F, L, R, false};
    return data;
}

SensorData
DetermineDustExistence(bool D)
{
    SensorData data = {false, false, false, D};
    return data;
}

/* ------------------------------------------------------------------------- */
/* WHEEL CONTROL FSM                                                         */
/* ------------------------------------------------------------------------- */

WheelState
WheelControl(WheelState wheel_state, SensorData obstacle_location, bool *cleaner_control_enable)
{
    bool F = obstacle_location.F;
    bool L = obstacle_location.L;
    bool R = obstacle_location.R;

    switch (wheel_state)
    {
    case W_STOP:
        if (F && !L)
        {
            TurnLeft();
            tickCount = 0;
            return W_TURN_LEFT;
        }
        if (F && L && R)
        {
            MoveBackward(true);
            tickCount = 0;
            return W_MOVE_BACK;
        }
        if (!F)
        {
            MoveForward(true);
            *cleaner_control_enable = true;
            tickCount = 0;
            return W_MOVE_FORWARD;
        }
        if (F && !R && L)
        {
            TurnRight();
            tickCount = 0;
            return W_TURN_RIGHT;
        }
        break;
    case W_MOVE_FORWARD:
        if (F)
        {
            MoveForward(false);
            *cleaner_control_enable = false;
            tickCount = 0;
            return W_STOP;
        }
        break;
    case W_MOVE_BACK:
        if (tickCount >= 5)
        {
            MoveBackward(false);
            TurnLeft();
            tickCount = 0;
            return W_TURN_LEFT;
        }
        break;
    case W_TURN_LEFT:
        if (tickCount >= 5)
        {
            MoveForward(true);
            *cleaner_control_enable = true;
            tickCount = 0;
            return W_MOVE_FORWARD;
        }
        break;
    }
    tickCount++;
    return wheel_state;
}

/* ------------------------------------------------------------------------- */
/* CLEANER CONTROL FSM                                                       */
/* ------------------------------------------------------------------------- */

CleanerState
CleanerControl(CleanerState cleaner_state, SensorData dust_existence, bool cleaner_control_enable)
{
    switch (cleaner_state)
    {
    case CLEANER_OFF:
        if (cleaner_control_enable)
        {
            Cleaner(ON);
            return CLEANER_ON;
        }
        break;
    case CLEANER_ON:
        if (dust_existence.D)
        {
            Cleaner(UP);
            return CLEANER_UP;
        }
        if (!cleaner_control_enable)
        {
            Cleaner(OFF);
            return CLEANER_OFF;
        }
        break;
    case CLEANER_UP:
        if (!dust_existence.D)
        {
            Cleaner(ON);
            return CLEANER_ON;
        }
        if (!cleaner_control_enable)
        {
            Cleaner(OFF);
            return CLEANER_OFF;
        }
        break;
    }
    // If the cleaner state does not change, return the current state.
    return cleaner_state;
}