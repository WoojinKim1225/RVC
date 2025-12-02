#include <gtest/gtest.h>
#include "../include/rvc.h"

extern "C"
{
    extern bool stub_front_input;
    extern int stub_left_input;
    extern int stub_right_input;
    extern int stub_dust_input;

    extern MotorCommand lastMotorCommand;
    extern int tickCount;
}

class WheelControlTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        tickCount = 0;
    }
};

TEST_F(WheelControlTest, TurnLeftState_IncrementsTickCountUntilThreshold)
{
    WheelState currentState = W_TURN_LEFT;
    SensorData obstacle = {false, false, false, false}; // 장애물 없음
    CleanerCommand cleaner_enable = OFF;

    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(tickCount, i);
        currentState = Controller(obstacle, currentState, &cleaner_enable);
        EXPECT_EQ(currentState, W_TURN_LEFT);
    }

    // tickCount가 5가 되면 상태가 W_MOVE_FORWARD로 변경되어야 합니다.
    EXPECT_EQ(tickCount, 5);
    currentState = Controller(obstacle, currentState, &cleaner_enable);
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
}
TEST_F(WheelControlTest, TurnRightState_IncrementsTickCountUntilThreshold)
{
    WheelState currentState = W_TURN_RIGHT;
    SensorData obstacle = {false, false, false, false};
    CleanerCommand cleaner_enable = OFF;

    // tickCount가 0에서 4로 증가하는 동안 상태는 W_TURN_LEFT를 유지해야 합니다.
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(tickCount, i);
        currentState = Controller(obstacle, currentState, &cleaner_enable);
        EXPECT_EQ(currentState, W_TURN_RIGHT);
    }

    // tickCount가 5가 되면 상태가 W_MOVE_FORWARD로 변경되어야 합니다.
    EXPECT_EQ(tickCount, 5);
    currentState = Controller(obstacle, currentState, &cleaner_enable);
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
}

TEST_F(WheelControlTest, TurnLeftState_WithDust_IncrementsTickCountUntilThreshold)
{
    WheelState currentState = W_TURN_LEFT;
    SensorData obstacle = {false, false, false, true};
    CleanerCommand cleaner_enable = OFF;

    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(tickCount, i);
        currentState = Controller(obstacle, currentState, &cleaner_enable);
        EXPECT_EQ(currentState, W_TURN_LEFT);
    }

    // tickCount가 5가 되면 상태가 W_MOVE_FORWARD로 변경되어야 합니다.
    EXPECT_EQ(tickCount, 5);
    currentState = Controller(obstacle, currentState, &cleaner_enable);
    EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
}

TEST_F(WheelControlTest, TurnRightState_WithDust_IncrementsTickCountUntilThreshold)
{
    WheelState currentState = W_TURN_RIGHT;
    SensorData obstacle = {false, false, false, true};
    CleanerCommand cleaner_enable = OFF;

    // tickCount가 0에서 4로 증가하는 동안 상태는 W_TURN_LEFT를 유지해야 합니다.
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(tickCount, i);
        currentState = Controller(obstacle, currentState, &cleaner_enable);
        EXPECT_EQ(currentState, W_TURN_RIGHT);
    }

    // tickCount가 5가 되면 상태가 W_MOVE_FORWARD로 변경되어야 합니다.
    EXPECT_EQ(tickCount, 5);
    currentState = Controller(obstacle, currentState, &cleaner_enable);
    EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
}