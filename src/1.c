#include <stdio.h>  
#include <stdlib.h> 
#include <stdbool.h>

// Controller
void Controller();

int Det_OL();
int Det_DE();
// 인터페이스
void Front_SI();
void Left_SI();
void Right_SI();
void Dust_SI();

//동작
void MoveForward();
void TurnLeft();
void TurnRight();
void MoveBackward();

// sensor input
bool ReadFrontSensor();
int ReadLeftSensor();
int ReadRightSensor();
int ReadDustSensor();

bool ReadFrontSensor(bool sensor_value){
    return sensor_value;
};
int ReadLeftSensor(int analog_value){
    if (analog_value < 100){
        return true;
    }
    return false;
};
int ReadRightSensor(int analog_value){
    if (analog_value < 100){
        return true;
    }
    return false;
};
int ReadDustSensor(int analog_value){
    if (analog_value < 100){
        return true;
    }
    return false;
};


typedef struct {
    bool F;   
    bool L;   
    bool R;  
    bool D; 
} SensorData;
enum MotorCommand {
    MOVE_FWD, MOVE_BACK, TURN_LEFT, TURN_RIGHT, STOP 
};
enum CleanerMommand {
    OFF, ON, UP
};


void main()
{
    int obstacle_Location;
    bool dust_Existence;

    while(1)
    {
        obstacle_Location = Det_OL();
        dust_Existence = Det_DE();

        wait(200);
    }
}