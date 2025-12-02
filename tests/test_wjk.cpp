#include <gtest/gtest.h>
#include "../include/rvc.h"

/* mock/stub 변수 및 함수 가져오기 (C linkage) */
extern "C"
{
    extern bool stub_front_input;
    extern int stub_left_input;
    extern int stub_right_input;
    extern int stub_dust_input;

    // Mock 함수가 기록하는 마지막 명령어를 가져오기 위한 변수
    extern MotorCommand lastMotorCommand;
    extern int tickCount;
}

/* --------------------------------------------------------- */
/* 1. FrontSensorInterface 테스트                             */
/* --------------------------------------------------------- */
TEST(Unit_SensorInterface, FrontSensor_ReturnsCorrectBinary)
{
    // front sensor = true
    stub_front_input = true;
    EXPECT_TRUE(FrontSensorInterface(ReadFrontSensor()));

    // front sensor = false
    stub_front_input = false;
    EXPECT_FALSE(FrontSensorInterface(ReadFrontSensor()));
}
