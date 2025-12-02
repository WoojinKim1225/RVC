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

WheelState Controller(SensorData data, WheelState wheelstate, CleanerCommand* com1);

/* Sensor-state determination */
SensorData DetermineObstacleLocation(bool F, bool L,bool R);
SensorData DetermineDustExistence(bool D);
SensorData Merge_Sensordata(SensorData obstacle, SensorData dust);

/* Sensor interface */
bool FrontSensorInterface(bool frontsensor_input);
bool LeftSensorInterface(int Leftsensor_input);
bool RightSensorInterface(int rightsensor_input);
bool DustSensorInterface(int dustsensor_input );

/* Actuators */
MotorCommand MoveForward(bool enable);
MotorCommand TurnLeft();
MotorCommand TurnRight();
MotorCommand MoveBackward(bool enable);
void Cleaner(CleanerCommand com);

int tickCount = 0;
/* Raw sensor input functions */
bool ReadFrontSensor();
int ReadLeftSensor();
int ReadRightSensor();
int ReadDustSensor();



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
WheelState Controller(SensorData data, WheelState wheelstate, CleanerCommand* com){
    bool F = data.F;
    bool L = data.L;
    bool D = data.D;
    bool R = data.R;
    switch(wheelstate){
        case W_STOP:
            if (F && L && R){
                MoveBackward(true);
                tickCount = 0;
                return W_MOVE_BACK;
            }else if(!F && D){
                MoveForward(true);
                tickCount = 0;
                *com = UP;
                return W_MOVE_FORWARD_UP;
            }else if(F && !R && L){
                TurnRight();
                tickCount = 0;
                return W_TURN_RIGHT;
            }else if(!F){
                MoveForward(true);
                tickCount = 0;
                *com = ON;
                return W_MOVE_FORWARD;
            }else if(F && !L){
                TurnLeft();
                tickCount = 0;
                return W_TURN_LEFT;
            }
            break;
        case W_MOVE_FORWARD:
            if (F){
                MoveBackward(false);
                tickCount = 0;
                *com = OFF;
                return W_STOP;
            }else if(D){
                *com = UP;
                tickCount = 0;
                return W_MOVE_FORWARD_UP;
            }
            break;
        case W_MOVE_BACK:
            if (!L){
                MoveBackward(false);
                TurnLeft();
                tickCount = 0;
                return W_TURN_LEFT;
            }else if(!R){
                MoveBackward(false);
                TurnRight();
                tickCount = 0;
                return W_TURN_RIGHT;
            }
            break;
        case W_TURN_LEFT:
            if (!D && tickCount >= 5){
                MoveForward(true);
                *com = ON;
                tickCount = 0;
                return W_MOVE_FORWARD;
            }else if(D && tickCount >= 5){
                MoveForward(true);
                *com = UP;
                tickCount = 0;
                return W_MOVE_FORWARD_UP;
            }
            break;
        case W_TURN_RIGHT:
            if (!D && tickCount >= 5){
                MoveForward(true);
                *com = ON;
                tickCount = 0;
                return W_MOVE_FORWARD;
            }else if(D && tickCount >= 5){
                MoveForward(true);
                *com = UP;
                tickCount = 0;
                return W_MOVE_FORWARD_UP;
            }
            break;
        case W_MOVE_FORWARD_UP:
            if (F){
                MoveForward(false);
                *com = OFF;
                tickCount = 0;
                return W_STOP;
            }else if(tickCount >= 5){
                *com = ON;
                tickCount = 0;
                return W_MOVE_FORWARD;
            }
            break;
    }
    tickCount ++;
    return wheelstate;
    
}


MotorCommand MoveForward(bool enable)
    {
        if (enable)
            return MOVE_FWD;
        else
            return STOP;
    }
    //test용 stub 구현(하드웨어 Turnleft)
    MotorCommand TurnLeft(){
        return TURN_LEFT;
    }
    //test용 stub 구현(하드웨어 Turnright)
    MotorCommand TurnRight(){
        return TURN_RIGHT;
    }
    //test용 stub구현(하드웨어 Movebackward)
    MotorCommand MoveBackward(bool enable){
        if(enable){
            return MOVE_BACK;
        }
        else{
            return STOP;
        }
    }
/* ------------------------------------------------------------------------- */
/* MAIN LOOP                                                                  */
/* ------------------------------------------------------------------------- */

#ifdef UNIT_TEST
int main(void)
{
    SensorData obstacle_Location;
    SensorData dust_Existence;
    SensorData merged_data;
    CleanerCommand cleaner_com = OFF;
    WheelState wheelstate = W_STOP;

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

        obstacle_Location = DetermineObstacleLocation(F,L,R);
        dust_Existence = DetermineDustExistence(D);

        /* Step 2: Merge obstacle + dust info */
        merged_data = Merge_Sensordata(obstacle_Location, dust_Existence);

        /* Step 3: Compute actuator commands */
        wheelstate = Controller(merged_data, wheelstate, &cleaner_com);
        Cleaner(cleaner_com);
        /* Step 4: Wait for next control tick */
        wait(TICK);
    }
    return 0;
}
#endif