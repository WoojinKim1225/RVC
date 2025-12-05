#include <gtest/gtest.h>
#include "rvc.h"

extern "C"
{
    extern bool stub_front_input;
    extern int stub_left_input;
    extern int stub_right_input;
    extern int stub_dust_input;
}

TEST(IntegrationTest, DustSensorToDetermine)
{

    stub_dust_input = 800;
    bool D = DustSensorInterface(stub_dust_input);
    SensorData data = DetermineDustExistence(D);
    EXPECT_TRUE(data.D);  // dust detected
    EXPECT_FALSE(data.F); // determineDust는 F/L/R 모두 false 초기화
    EXPECT_FALSE(data.L);
    EXPECT_FALSE(data.R);

    stub_dust_input = 600;
    D = DustSensorInterface(stub_dust_input);
    data = DetermineDustExistence(D);
    EXPECT_FALSE(data.D); // dust detected
    EXPECT_FALSE(data.F); // determineDust는 F/L/R 모두 false 초기화
    EXPECT_FALSE(data.L);
    EXPECT_FALSE(data.R);
}

TEST(IntegrationTest, FrontSensorToDetermineObstacle)
{

    stub_front_input = true;
    bool F = FrontSensorInterface(stub_front_input);
    SensorData data = DetermineObstacleLocation(F, false, false);
    // 기대 결과
    EXPECT_TRUE(data.F);
    EXPECT_FALSE(data.L);
    EXPECT_FALSE(data.R);
    EXPECT_FALSE(data.D);

    stub_front_input = false;
    F = FrontSensorInterface(stub_front_input);
    data = DetermineObstacleLocation(F, false, false);
    // 기대 결과
    EXPECT_FALSE(data.F);
    EXPECT_FALSE(data.L);
    EXPECT_FALSE(data.R);
    EXPECT_FALSE(data.D);
}

TEST(IntegrationTest, RightSensorToDetermineObstacle)
{

    stub_right_input = 120; // <100 → true
    bool R = RightSensorInterface(stub_right_input);
    SensorData data = DetermineObstacleLocation(false, false, R);
    EXPECT_FALSE(data.F);
    EXPECT_FALSE(data.L);
    EXPECT_FALSE(data.R);
    EXPECT_FALSE(data.D);

    stub_right_input = 100; // <100 → true
    R = RightSensorInterface(stub_right_input);
    data = DetermineObstacleLocation(false, false, R);
    EXPECT_FALSE(data.F);
    EXPECT_FALSE(data.L);
    EXPECT_FALSE(data.R);
    EXPECT_FALSE(data.D);

    stub_right_input = 99; // <100 → true
    R = RightSensorInterface(stub_right_input);
    data = DetermineObstacleLocation(false, false, R);
    EXPECT_FALSE(data.F);
    EXPECT_FALSE(data.L);
    EXPECT_TRUE(data.R);
    EXPECT_FALSE(data.D);
}
