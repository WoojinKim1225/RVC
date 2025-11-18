#include <stdio.h>  
#include <stdlib.h> 
#include <stdbool.h>

#define TICK 10

// Controller
CleanerCommand Controller();

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

CleanerCommand Controller(SensorData data){
    CleanerCommand Cleaner_com = OFF;
    if (!data.F){
        MoveForward(true);
        Cleaner_com = ON;
    }else if(data.F && data.L && data.R){
        MoveBackward(true);
    }else if(data.F && !data.L){
        TurnLeft();
    }else if(data.F && data.L && !data.R){
        TurnRight();
    }

    if (data.D){
        CleanerCommand Cleaner_com = UP;
    }
    return Cleaner_com;
}
void main()
{
    int obstacle_Location;
    bool dust_Existence;

    while(1)
    {
        obstacle_Location = DetermineObstacleLocation();
        dust_Existence = DetermineDustExistence();

        wait(TICK);
    }
}