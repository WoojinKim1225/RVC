#include <stdio.h>  
#include <stdlib.h> 
#include <stdbool.h>

#define TICK 10

/* ------------------------------------------------------------------------- */
/* DATA STRUCTURES                                                           */
/* ------------------------------------------------------------------------- */

/* SensorData: 각 센서의 binary interpreted 상태를 저장 */
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

/* ------------------------------------------------------------------------- */
/* CONTROLLER INTERFACE                                                      */
/* ------------------------------------------------------------------------- */

CleanerCommand Controller();

/* Sensor-state determination */
SensorData DetermineObstacleLocation();
SensorData DetermineDustExistence();
SensorData Merge_Sensordata();

/* Sensor interface */
bool FrontSensorInterface(bool sensor_value);
bool LeftSensorInterface(int analog_value);
bool RightSensorInterface(int analog_value);
bool DustSensorInterface(int analog_value);

/* Actuators */
MotorCommand MoveForward(bool enable);
MotorCommand TurnLeft();
MotorCommand TurnRight();
MotorCommand MoveBackward(bool enable);

/* Raw sensor input functions */
bool ReadFrontSensor(void);
int ReadLeftSensor(void);
int ReadRightSensor(void);
int ReadDustSensor(void);

int tickCount = 0;


/* ------------------------------------------------------------------------- */
/* MAIN LOOP                                                                  */
/* ------------------------------------------------------------------------- */

int 
main(void)
{
    SensorData obstacle_Location;
    SensorData dust_Existence;
    SensorData merged_data;
    CleanerCommand cleaner_com = OFF;

    while (1)
    {
        bool rawF = ReadFrontSensor();
        int rawL = ReadLeftSensor();
        int rawR = ReadRightSensor();
        int rawD = ReadDustSensor();

        bool F = FrontSensorInterface(rawF);
        bool L = LeftSensorInterface(rawL);
        bool R = RightSensorInterface(rawR);
        bool D = DustSensorInterface(rawD);

        obstacle_Location = DetermineObstacleLocation();
        dust_Existence = DetermineDustExistence();

        merged_data = Merge_Sensordata(obstacle_Location, dust_Existence);

        cleaner_com = Controller(merged_data);

        wait(TICK);
    }
}


/* ------------------------------------------------------------------------- */
/* SENSOR INTERFACE IMPLEMENTATION                                           */
/* ------------------------------------------------------------------------- */

bool 
FrontSensorInterface(bool sensor_value){
    return sensor_value;
}

bool 
LeftSensorInterface(int analog_value){
    return (analog_value < 100);
}

bool 
RightSensorInterface(int analog_value){
    return (analog_value < 100);
}

bool 
DustSensorInterface(int analog_value){
    return (analog_value > 600);
}

/* ------------------------------------------------------------------------- */
/* SENSOR STATE DETERMINATION                                                */
/* ------------------------------------------------------------------------- */

SensorData 
DetermineObstacleLocation(bool F, bool L, bool R){
    SensorData data = {F, L, R, false};
    return data;
}

SensorData 
DetermineDustExistence(bool D){
    SensorData data = {false, false, false, D};
    return data;
}

/*  두 종류의 센서 정보(장애물/먼지)를 하나의 데이터로 병합 */
SensorData 
Merge_Sensordata(SensorData obstacle, SensorData dust){
    SensorData data = obstacle;
    data.D = dust.D;
    return data;
}

/* ------------------------------------------------------------------------- */
/* MAIN CONTROLLER FSM                                                       */
/*   - 장애물 여부에 따라 이동 방향 결정                                      */
/*   - 먼지가 존재하면 Cleaner UP 출력                                        */
/* ------------------------------------------------------------------------- */

CleanerCommand 
Controller(SensorData data)
{
    CleanerCommand cleaner_com = OFF;

    /* Wheel motion control */
    if (!data.F) {
        /* No obstacle front → forward movement + cleaner ON */
        MoveForward(true);
        cleaner_com = ON;

    } else if (data.F && data.L && data.R) {
        /* All blocked → move backward */
        MoveBackward(true);

    } else if (data.F && !data.L) {
        /* Left side open → turn left */
        TurnLeft();

    } else if (data.F && data.L && !data.R) {
        /* Right side open → turn right */
        TurnRight();
    }

    /* Cleaner state control */
    if (data.D) {
        /* Dust detected → cleaner UP */
        cleaner_com = UP;
    }

    return cleaner_com;
}