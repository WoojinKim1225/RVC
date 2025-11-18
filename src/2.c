#include <stdio.h>  
#include <stdlib.h> 
#include <stdbool.h>

#define TICK 10

// Controller
void WheelController();
CleanerState CleanerControl(CleanerState, bool, bool);

int DetermineObstacleLocation();
int DetermineDustExistence();
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

void 
main()
{
    int obstacle_location;
    bool dust_existence;

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
CleanerControl(CleanerState cleaner_state, bool dust_existence, bool cleaner_control_enable)
{
    switch (cleaner_state) 
    {
        case CLEANER_OFF:
            if (cleaner_control_enable) {
                CleanerOn();
                cleaner_state = CLEANER_ON;
            }
        break;
        case CLEANER_ON:
            if (dust_existence) {
                CleanerUp();
                cleaner_state = CLEANER_UP;
            } else if (!cleaner_control_enable) {
                CleanerOff();
                cleaner_state = CLEANER_OFF;
            }
            break;
        break;
        case CLEANER_UP:
            if (!dust_existence) {
                CleanerOn();
                cleaner_state = CLEANER_ON;
            } else if (!cleaner_control_enable) {
                CleanerOff();
                cleaner_state = CLEANER_OFF;
            }
        break;
    }
}