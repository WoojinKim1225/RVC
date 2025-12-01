#ifndef RVC_H
#define RVC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/* ------------------------------------------------------------------------- */
/* DATA STRUCTURES                                                           */
/* ------------------------------------------------------------------------- */

typedef struct {
    bool F;   /* Front obstacle sensor  */
    bool L;   /* Left obstacle sensor   */
    bool R;   /* Right obstacle sensor  */
    bool D;   /* Dust detection sensor  */
} SensorData;

/* Wheel motor command enum */
typedef enum {
    MOVE_FWD,
    MOVE_BACK,
    TURN_LEFT,
    TURN_RIGHT,
    STOP
} MotorCommand;

/* Cleaner actuator command enum */
typedef enum {
    OFF,
    ON,
    UP
} CleanerCommand;

/* WheelState: FSM states */
typedef enum {
    W_STOP,
    W_MOVE_FORWARD,
    W_MOVE_BACK,
    W_TURN_LEFT,
    W_TURN_RIGHT,
    W_MOVE_FORWARD_UP
} WheelState;

/* ------------------------------------------------------------------------- */
/* CONTROLLER INTERFACE                                                      */
/* ------------------------------------------------------------------------- */

/* FSM */
extern WheelState Controller(SensorData data, WheelState wheelstate, CleanerCommand* com);

/* Sensor-state determination */
extern SensorData DetermineObstacleLocation(bool F, bool L, bool R);
extern SensorData DetermineDustExistence(bool D);
extern SensorData Merge_Sensordata(SensorData obstacle, SensorData dust);

/* Sensor interface */
extern bool FrontSensorInterface(bool frontsensor_input);
extern bool LeftSensorInterface(int Leftsensor_input);
extern bool RightSensorInterface(int rightsensor_input);
extern bool DustSensorInterface(int dustsensor_input);

/* Actuator interface (will be mocked in tests) */
extern MotorCommand MoveForward(bool enable);
extern MotorCommand MoveBackward(bool enable);
extern MotorCommand TurnLeft();
extern MotorCommand TurnRight();
extern void Cleaner(CleanerCommand com);

/* Sensor raw input functions (will be stubbed/mocked in tests) */
extern bool ReadFrontSensor();
extern int ReadLeftSensor();
extern int ReadRightSensor();
extern int ReadDustSensor();

/* Global tickCount */
extern int tickCount;

#ifdef __cplusplus
}
#endif

#endif