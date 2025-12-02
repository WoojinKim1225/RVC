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

/* --------------------------------------------------------- */
/* 2. WheelControl FSM 테스트                                 */
/* --------------------------------------------------------- */
class WheelControlTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 각 테스트 전에 tickCount를 0으로 초기화합니다.
        tickCount = 0;
    }
};

// W_TURN_LEFT 상태에서 tickCount가 5가 될 때까지 증가하는지 테스트합니다.
TEST_F(WheelControlTest, TurnLeftState_IncrementsTickCountUntilThreshold)
{
    WheelState currentState = W_TURN_LEFT;
    SensorData obstacle = {false, false, false, false}; // 장애물 없음
    bool cleaner_enable = false;

    // tickCount가 0에서 4로 증가하는 동안 상태는 W_TURN_LEFT를 유지해야 합니다.
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(tickCount, i);
        currentState = WheelControl(currentState, obstacle, &cleaner_enable);
        EXPECT_EQ(currentState, W_TURN_LEFT);
    }

    // tickCount가 5가 되면 상태가 W_MOVE_FORWARD로 변경되어야 합니다.
    EXPECT_EQ(tickCount, 5);
    currentState = WheelControl(currentState, obstacle, &cleaner_enable);
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
}

// W_MOVE_BACK 상태에서 tickCount가 5가 되면 W_TURN_LEFT로 변경되는지 테스트합니다.
TEST_F(WheelControlTest, MoveBackState_TransitionsToTurnLeftAfterTimeout)
{
    WheelState currentState = W_MOVE_BACK;
    SensorData obstacle = {false, false, false, false}; // 장애물 없음
    bool cleaner_enable = false;

    // tickCount가 0에서 4로 증가하는 동안 상태는 W_MOVE_BACK을 유지해야 합니다.
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(tickCount, i);
        currentState = WheelControl(currentState, obstacle, &cleaner_enable);
        EXPECT_EQ(currentState, W_MOVE_BACK);
    }

    // tickCount가 5가 되면 상태가 W_TURN_LEFT로 변경되어야 합니다.
    EXPECT_EQ(tickCount, 5);
    currentState = WheelControl(currentState, obstacle, &cleaner_enable);
    EXPECT_EQ(currentState, W_TURN_LEFT);
    EXPECT_EQ(tickCount, 0); // 상태 변경 후 tickCount가 0으로 리셋되는지 확인
}
