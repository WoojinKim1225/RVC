#include <stdio.h>  
#include <stdlib.h> 
#include <stdbool.h>

#define TICK 10

// Controller
WheelState Controller();

SensorData DetermineObstacleLocation();
SensorData DetermineDustExistence();
SensorData Merge_Sensordata();
// 인터페이스
bool FrontSensorInterface();
bool LeftSensorInterface();
bool RightSensorInterface();
bool DustSensorInterface();

//동작
MotorCommand MoveForward(bool enable);
MotorCommand TurnLeft();
MotorCommand TurnRight();
MotorCommand MoveBackward(bool enable);
void Cleaner(CleanerCommand com);


int tickCount = 0;

// sensor input
bool ReadFrontSensor();
int ReadLeftSensor();
int ReadRightSensor();
int ReadDustSensor();
typedef struct {
    bool F;   
    bool L;   
    bool R;  
    bool D; 
} SensorData;
typedef enum {
    MOVE_FWD, MOVE_BACK, TURN_LEFT, TURN_RIGHT, STOP 
}MotorCommand;
typedef enum {
    OFF, ON, UP
}CleanerCommand;
typedef enum{
    STOP,
    MOVE_FORWARD,
    MOVE_BACK,
    TURN_LEFT,
    TURN_RIGHT,
    MOVE_FORWARD_UP
} WheelState;


bool FrontSensorInterface(bool sensor_value){
    return sensor_value;
}
bool LeftSensorInterface(int analog_value){
    if (analog_value < 100){
        return true;
    }
    return false;
}
bool RightSensorInterface(int analog_value){
    if (analog_value < 100){
        return true;
    }
    return false;
}
bool DustSensorInterface(int analog_value){
    if (analog_value > 600){
        return true;
    }
    return false;
}
SensorData DetermineObstacleLocation(bool F, bool L, bool R){
    SensorData data = {F,L,R,false};
    return data;
}
SensorData DetermineDustExistence(bool D){
    SensorData data = {false, false, false, D};
    return data;
}
SensorData Merge_Sensordata(SensorData obstacle, SensorData Dust){
    SensorData data = obstacle;
    data.D = Dust.D;
    return data;
}

WheelState Controller(SensorData data, WheelState wheelstate, CleanerCommand com1){
    bool F = data.F;
    bool L = data.L;
    bool D = data.D;
    bool R = data.R;
    CleanerCommand com = com1;
    switch(wheelstate){
        case STOP:
            if (F && L && R){
                MoveBackward(true);
                tickCount = 0;
                return MOVE_BACK;
            }else if(!F && D){
                MoveForward(true);
                tickCount = 0;
                com = UP;
                return MOVE_FORWARD_UP;
            }else if(F && !R && L){
                TurnRight();
                tickCount = 0;
                return TURN_RIGHT;
            }else if(!F){
                MoveForward();
                tickCount = 0;
                com = ON;
                return MOVE_FORWARD
            }else if(F && !L){
                TurnLeft();
                tickCount = 0;
                return TURN_LEFT;
            }
            break;
        case MOVE_FORWARD:
            if (F){
                MoveBackward(false);
                tickCount = 0;
                return STOP;
            }else if(D){
                com = UP;
                tickCount = 0;
                return MOVE_FORWARD_UP;
            }
            break;
        case MOVE_BACK:
            if (!L){
                MoveBackward(false);
                TurnLeft();
                tickCount = 0;
                return TURN_LEFT;
            }else if(!R){
                MoveBackward(false);
                TurnRight();
                tickCount = 0;
                return TURN_RIGHT;
            }
            break;
        case TURN_LEFT:
            if (!D && tickCount >= 5){
                MoveForward(true);
                com = ON;
                tickCount = 0;
                return MOVE_FORWARD;
            }else if(D && tickCount >= 5){
                MoveForward(true);
                com = UP;
                tickCount = 0;
                return MOVE_FORWARD_UP;
            }
            break;
        case TURN_RIGHT:
            if (!D && tickCount >= 5){
                MoveForward(true);
                com = ON;
                tickCount = 0;
                return MOVE_FORWARD;
            }else if(D && tickCount >= 5){
                MoveForward(true);
                com = UP;
                tickCount = 0;
                return MOVE_FORWARD_UP;
            }
            break;
        case MOVE_FORWARD_UP:
            if (F){
                MoveForward(false);
                com = OFF;
                tickCount = 0;
                return STOP;
            }else if(tickCount >= 5){
                com = ON;
                tickCount = 0;
                return MOVE_FORWARD;
            }
            break;
    }
    tickCount ++;
    Clenaer(com);


}
void main()
{
    SensorData obstacle_Location;
    SensorData dust_Existence;
    SensorData Merge_data;
    CleanerCommand cleaner_com;
    WheelState wheelstate = STOP;
    while(1)
    {
        obstacle_Location = DetermineObstacleLocation();
        dust_Existence = DetermineDustExistence();
        Merge_data = Merge_Sensordata(obstacle_Location,DetermineDustExistence);
        wheelstate = Controller(Merge_data, wheelstate);

        wait(TICK);
    }
}