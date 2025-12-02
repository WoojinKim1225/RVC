#include "rvc.h"

// 테스트 할 떄 사용할 input 값들
bool stub_front_input = false;
int stub_left_input = 150;
int stub_right_input = 150;
int stub_dust_input = 0;

// test 할 때 모터 명령과 cleaner 명령을 기록할 수 있는 가짜 함수를 위한 변수
MotorCommand lastMotorCommand;
CleanerCommand lastCleanerCommand;

/* -------------------------------
   Stubbed sensor functions
   ------------------------------- */

bool ReadFrontSensor()
{
    return stub_front_input;
}

int ReadLeftSensor()
{
    return stub_left_input;
}

int ReadRightSensor()
{
    return stub_right_input;
}

int ReadDustSensor()
{
    return stub_dust_input;
}

MotorCommand MoveForward(bool enable)
{
    lastMotorCommand = MOVE_FWD;
    return MOVE_FWD;
}

MotorCommand MoveBackward(bool enable)
{
    lastMotorCommand = MOVE_BACK;
    return MOVE_BACK;
}

MotorCommand TurnLeft()
{
    lastMotorCommand = TURN_LEFT;
    return TURN_LEFT;
}

MotorCommand TurnRight()
{
    lastMotorCommand = TURN_RIGHT;
    return TURN_RIGHT;
}

void Cleaner(CleanerCommand com)
{
    lastCleanerCommand = com;
}