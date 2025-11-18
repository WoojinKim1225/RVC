#include <stdio.h>  
#include <stdlib.h> 
#include <stdbool.h>

#define TICK 10

// Controller
void WheelController();
CleanerState CleanerControl(CleanerState, SensorData, bool);

SensorData DetermineObstacleLocation();
SensorData DetermineDustExistence();
// 인터페이스
void FrontSensorInterface();
void LeftSensorInterface();
void RightSensorInterface();
void DustSensorInterface();

//동작
void MoveForward();
void TurnLeft();
void TurnRight();
void MoveBackward();
void CleanerOff();
void CleanerOn();
void CleanerUp();

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
    CLEANER_OFF,
    CLEANER_ON,
    CLEANER_UP
} CleanerState;

enum MotorCommand {
    MOVE_FWD, MOVE_BACK, TURN_LEFT, TURN_RIGHT, STOP 
};

enum CleanerCommand {
    OFF, ON, UP
};


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

void main()
{
    SensorData obstacle_location;
    SensorData dust_existence;

    bool cleaner_control_enable;
    CleanerState cleaner_state = CLEANER_OFF;

    while(1)
    {
        obstacle_location = DetermineObstacleLocation();
        dust_existence = DetermineDustExistence();
        
        cleaner_state = CleanerControl(cleaner_state, dust_existence, cleaner_control_enable);

        wait(TICK);
    }
}

CleanerState
CleanerControl(CleanerState cleaner_state, SensorData dust_existence, bool cleaner_control_enable)
{
    switch (cleaner_state) 
    {
        case CLEANER_OFF:
            if (cleaner_control_enable) {
                CleanerOn();
                return CLEANER_ON;
            }
        break;
        case CLEANER_ON:
            if (dust_existence.D) {
                CleanerUp();
                return CLEANER_UP;
            } else if (!cleaner_control_enable) {
                CleanerOff();
                return CLEANER_OFF;
            }
            break;
        break;
        case CLEANER_UP:
            if (!dust_existence.D) {
                CleanerOn();
                return CLEANER_ON;
            } else if (!cleaner_control_enable) {
                CleanerOff();
                return CLEANER_OFF;
            }
        break;
    }
    // cleaner state가 안바뀌므로, 기존의 스테이트를 return.
    return cleaner_state;
}