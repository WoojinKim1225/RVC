#include <gtest/gtest.h>
#include "../include/rvc.h"

extern "C"
{
    extern bool stub_front_input;
    extern int stub_left_input;
    extern int stub_right_input;
    extern int stub_dust_input;

    extern int tickCount;

    extern MotorCommand lastMotorCommand;
    extern CleanerCommand lastCleanerCommand;
}

class UnitTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        tickCount = 0;
    }
};

TEST_F(UnitTest, TurnLeftState_IncrementsTickCountUntilThreshold)
{
    WheelState currentState = W_TURN_LEFT;
    SensorData obstacle = {false, false, false, false};
    CleanerCommand cleaner_enable = OFF;

    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(tickCount, i);
        currentState = Controller(obstacle, currentState, &cleaner_enable);
        EXPECT_EQ(currentState, W_TURN_LEFT);
    }

    EXPECT_EQ(tickCount, 5);
    currentState = Controller(obstacle, currentState, &cleaner_enable);
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
}
TEST_F(UnitTest, TurnRightState_IncrementsTickCountUntilThreshold)
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

TEST_F(UnitTest, TurnLeftState_WithDust_IncrementsTickCountUntilThreshold)
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

    // tickCount가 5가 되면 상태가 W_MOVE_FORWARD
    EXPECT_EQ(tickCount, 5);
    currentState = Controller(obstacle, currentState, &cleaner_enable);
    EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
}

TEST_F(UnitTest, TurnRightState_WithDust_IncrementsTickCountUntilThreshold)
{
    WheelState currentState = W_TURN_RIGHT;
    SensorData obstacle = {false, false, false, true};
    CleanerCommand cleaner_enable = OFF;

    // tickCount가 0에서 4로 증가하는 동안 상태는 W_TURN_LEFT
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(tickCount, i);
        currentState = Controller(obstacle, currentState, &cleaner_enable);
        EXPECT_EQ(currentState, W_TURN_RIGHT);
    }

    // tickCount가 5가 되면 상태가 W_MOVE_FORWARD
    EXPECT_EQ(tickCount, 5);
    currentState = Controller(obstacle, currentState, &cleaner_enable);
    EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
}

// W_MOVE_FORWARD_UP 상태에서 장애물과 먼지가 없을 때, 5틱 후에 W_MOVE_FORWARD로 상태가 변경되는지 테스트합니다.
TEST_F(UnitTest, MoveForwardUp_NoObstacleOrDust_TransitionsToMoveForwardAfterTimeout)
{
    WheelState currentState = W_MOVE_FORWARD_UP;
    SensorData sensor_data = {false, false, false, false}; // 장애물 및 먼지 없음
    CleanerCommand cleaner_enable = UP;                    // 초기 청소기 상태는 UP

    // tickCount가 0에서 4로 증가하는 동안 상태는 W_MOVE_FORWARD_UP을 유지해야 합니다.
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(tickCount, i);
        currentState = Controller(sensor_data, currentState, &cleaner_enable);
        EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
    }

    // tickCount가 5가 되면 상태가 W_MOVE_FORWARD로 변경되어야 합니다.
    EXPECT_EQ(tickCount, 5);
    currentState = Controller(sensor_data, currentState, &cleaner_enable);
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
}

//=========================================INTEGRATION================================================

class IntegrationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        tickCount = 0;
    }
};

using ParamType = SensorData;

class ControllerForwardIntegrationTest : public testing::TestWithParam<ParamType>
{
protected:
    // 필요하다면 SetUp() 또는 TearDown() 메소드를 정의할 수 있습니다.
    void SetUp() override
    {
        // 모든 테스트 실행 전에 필요한 초기화 로직
        lastMotorCommand = MOVE_FWD;
    }
};

// controller에 obstacle location과 dust existence 가 들어올 때
// 출력 move forward가 motor interface에 가서 direction이 잘 나오는지
TEST_P(ControllerForwardIntegrationTest, StaysInMoveForwardState)
{
    // Given: 테스트에 할당된 센서 데이터를 가져옵니다.
    SensorData sensor_data = GetParam();

    WheelState currentState = W_MOVE_FORWARD;
    CleanerCommand cleaner_enable = ON;

    // lastMotorCommand는 SetUp()에서 초기화됨 (MOVE_FWD)

    // When: Controller 함수 실행
    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    // Then: 상태 유지 및 모터 명령 확인
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD); // lastMotorCommand는 안 바뀌어야 함
    EXPECT_EQ(cleaner_enable, ON);         // cleaner도 안 바뀌어야 함
}

TEST_P(ControllerForwardIntegrationTest, MoveForwardUp_NoObstacleOrDust_CallsMotorInterfaceCorrectlyAfterTimeout)
{
    WheelState currentState = W_MOVE_FORWARD_UP;
    SensorData sensor_data = GetParam();
    CleanerCommand cleaner_enable = UP; // 초기 청소기 상태는 UP
    lastMotorCommand = MOVE_FWD;

    // tickCount가 0에서 4로 증가하는 동안 상태는 W_MOVE_FORWARD_UP을 유지해야 합니다.
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(tickCount, i);
        currentState = Controller(sensor_data, currentState, &cleaner_enable);
        EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
        EXPECT_EQ(lastMotorCommand, MOVE_FWD);
    }

    // tickCount가 5가 되면 상태가 W_MOVE_FORWARD로 변경되어야 합니다.
    EXPECT_EQ(tickCount, 5);
    currentState = Controller(sensor_data, currentState, &cleaner_enable);
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD);
    EXPECT_EQ(cleaner_enable, ON); // 청소기 모드가 ON으로 변경되었는지 확인
    EXPECT_EQ(tickCount, 0);       // tickCount가 0으로 리셋되었는지 확인
}

INSTANTIATE_TEST_SUITE_P(
    MoveForwardTestCases,
    ControllerForwardIntegrationTest,
    testing::Values(
        //{is_front_blocked, is_right_blocked, is_left_blocked, is_dust_detected}
        ParamType{false, false, false, false},
        ParamType{false, true, false, false},
        ParamType{false, false, true, false},
        ParamType{false, true, true, false}));

class ControllerStopIntegrationTest : public testing::TestWithParam<ParamType>
{
protected:
    // 필요하다면 SetUp() 또는 TearDown() 메소드를 정의할 수 있습니다.
    void SetUp() override
    {
        // 모든 테스트 실행 전에 필요한 초기화 로직
        lastMotorCommand = STOP;
    }
};

TEST_P(ControllerStopIntegrationTest, StopToMoveForwardState_CallsMotorInterfaceCorrectly)
{
    // Given: 테스트에 할당된 센서 데이터를 가져옵니다.
    SensorData sensor_data = GetParam();

    WheelState currentState = W_STOP;
    CleanerCommand cleaner_enable = OFF;

    // lastMotorCommand는 SetUp()에서 초기화됨 (MOVE_FWD)

    // When: Controller 함수 실행
    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    // Then: 상태 유지 및 모터 명령 확인
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD); // lastMotorCommand는 안 바뀌어야 함
    EXPECT_EQ(cleaner_enable, ON);         // cleaner도 안 바뀌어야 함
}

INSTANTIATE_TEST_SUITE_P(
    StopToMoveForwardState_CallsMotorInterfaceCorrectly,
    ControllerStopIntegrationTest,
    testing::Values(
        ParamType{false, false, false, false},
        ParamType{false, true, false, false},
        ParamType{false, false, true, false},
        ParamType{false, true, true, false}));

class ControllerStopDustIntegrationTest : public testing::TestWithParam<ParamType>
{
protected:
    // 필요하다면 SetUp() 또는 TearDown() 메소드를 정의할 수 있습니다.
    void SetUp() override
    {
        // 모든 테스트 실행 전에 필요한 초기화 로직
        lastMotorCommand = STOP;
    }
};

// W_STOP 상태에서 먼지 감지 시 W_MOVE_FORWARD_UP으로 전환되는지 테스트합니다.
TEST_P(ControllerStopDustIntegrationTest, StopToMoveForwardUpState_CallsMotorInterfaceCorrectly)
{
    // Given: 테스트에 할당된 센서 데이터를 가져옵니다.
    SensorData sensor_data = GetParam();

    WheelState currentState = W_STOP;
    CleanerCommand cleaner_enable = OFF;

    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    // Then: 상태 전환 및 모터/클리너 명령 확인
    EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD); // MoveForward(true)가 호출되어야 함
    EXPECT_EQ(cleaner_enable, UP);         // Cleaner가 UP으로 변경되어야 함
}

INSTANTIATE_TEST_SUITE_P(
    StopToMoveForwardUpCases,
    ControllerStopDustIntegrationTest,
    testing::Values(
        //{F, L, R, D} -> !F && D 조건을 만족하는 4가지 경우
        ParamType{false, false, false, true},
        ParamType{false, true, false, true},
        ParamType{false, false, true, true},
        ParamType{false, true, true, true}));

TEST_F(IntegrationTest, ForwardToForwardUpState_CallsMotorInterfaceCorrectly)
{
    // Given: 초기 상태와 센서 데이터 설정
    WheelState currentState = W_MOVE_FORWARD;
    SensorData sensor_data = {false, false, false, true}; // 장애물 없음, 먼지 있음
    CleanerCommand cleaner_enable = ON;
    lastMotorCommand = MOVE_FWD; // lastMotorCommand 초기화

    // When: Controller 함수 실행
    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    // Then: 상태 유지 및 모터 명령 확인
    EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD); // MotorInterface를 통해 MoveForward가 호출되었는지 확인
    EXPECT_EQ(cleaner_enable, UP);         // cleaner는 power up
}

// W_MOVE_FORWARD_UP 상태에서 장애물과 먼지가 없을 때, 5틱 후에 W_MOVE_FORWARD로 상태가 변경되는지 테스트합니다.

// W_MOVE_FORWARD_UP 상태에서 전방 장애물 감지 시 W_STOP으로 전환되는지 테스트합니다.
TEST_F(IntegrationTest, MoveForwardUpToStopTransition_CallsMotorInterfaceCorrectly)
{
    // Given: 초기 상태와 센서 데이터 설정
    WheelState currentState = W_MOVE_FORWARD_UP;
    SensorData sensor_data = {true, false, false, false}; // 전방 장애물 감지
    CleanerCommand cleaner_enable = UP;                   // 초기 청소기 상태는 UP
    lastMotorCommand = MOVE_FWD;                          // 초기 모터 명령은 전진

    // When: Controller 함수 실행
    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    // Then: 상태 전환 및 모터 명령 확인
    EXPECT_EQ(currentState, W_STOP);   // W_STOP 상태로 전환되었는지 확인
    EXPECT_EQ(lastMotorCommand, STOP); // MoveForward(false) 호출로 STOP 명령이 내려졌는지 확인
    EXPECT_EQ(cleaner_enable, OFF);    // Cleaner가 OFF로 변경되었는지 확인
    EXPECT_EQ(tickCount, 0);           // tickCount가 0으로 리셋되었는지 확인
}

// W_STOP 상태에서 전방과 좌측 장애물 감지 시 W_TURN_RIGHT로 전환되는지 테스트합니다.
TEST_F(IntegrationTest, StopToTurnRightTransition_CallsMotorInterfaceCorrectly)
{
    // Given: 초기 상태와 센서 데이터 설정
    WheelState currentState = W_STOP;
    // 조건: F=true, L=true, R=false
    SensorData sensor_data = {true, true, false, false};
    CleanerCommand cleaner_enable = OFF; // 초기 청소기 상태는 OFF
    lastMotorCommand = STOP;             // 초기 모터 명령은 STOP

    // When: Controller 함수 실행
    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    // Then: 상태 전환 및 모터 명령 확인
    EXPECT_EQ(currentState, W_TURN_RIGHT);   // W_TURN_RIGHT 상태로 전환되었는지 확인
    EXPECT_EQ(lastMotorCommand, TURN_RIGHT); // TurnRight() 호출로 TURN_RIGHT 명령이 내려졌는지 확인
    EXPECT_EQ(cleaner_enable, OFF);          // Cleaner 상태는 변하지 않아야 함
    EXPECT_EQ(tickCount, 0);                 // tickCount가 0으로 리셋되었는지 확인
}

// W_STOP 상태에서 전방 장애물 감지 시 W_TURN_LEFT로 전환되는지 테스트합니다.
TEST_F(IntegrationTest, StopToTurnLeftTransition_CallsMotorInterfaceCorrectly)
{
    // Given: 초기 상태와 센서 데이터 설정
    WheelState currentState = W_STOP;
    // 조건: F=true, L=false (다른 조건들은 이전 if문을 통과하도록 설정)
    SensorData sensor_data = {true, false, true, false};
    CleanerCommand cleaner_enable = OFF; // 초기 청소기 상태는 OFF
    lastMotorCommand = STOP;             // 초기 모터 명령은 STOP

    // When: Controller 함수 실행
    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    // Then: 상태 전환 및 모터 명령 확인
    EXPECT_EQ(currentState, W_TURN_LEFT);   // W_TURN_LEFT 상태로 전환되었는지 확인
    EXPECT_EQ(lastMotorCommand, TURN_LEFT); // TurnLeft() 호출로 TURN_LEFT 명령이 내려졌는지 확인
    EXPECT_EQ(cleaner_enable, OFF);         // Cleaner 상태는 변하지 않아야 함
    EXPECT_EQ(tickCount, 0);                // tickCount가 0으로 리셋되었는지 확인
}

class ControllerMoveForwardToStopIntegrationTest : public testing::TestWithParam<ParamType>
{
protected:
    void SetUp() override
    {
        // W_MOVE_FORWARD 상태의 초기 조건을 설정합니다.
        lastMotorCommand = MOVE_FWD;
        tickCount = 0;
    }
};

// W_MOVE_FORWARD 상태에서 전방 장애물 감지 시 W_STOP으로 전환되는지 테스트합니다.
// L, R, D 센서 값에 관계없이 동일하게 동작해야 합니다.
TEST_P(ControllerMoveForwardToStopIntegrationTest, MoveForwardToStopTransition_CallsMotorInterfaceCorrectly)
{
    // Given: 테스트 매개변수로부터 센서 데이터를 가져옵니다.
    SensorData sensor_data = GetParam();
    WheelState currentState = W_MOVE_FORWARD;
    CleanerCommand cleaner_enable = ON;

    // When: Controller 함수를 실행합니다.
    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    // Then: 상태 전환 및 모터/클리너 명령을 확인합니다.
    EXPECT_EQ(currentState, W_STOP);
    EXPECT_EQ(lastMotorCommand, STOP); // MoveBackward(false)가 호출되어 STOP이 되어야 함
    EXPECT_EQ(cleaner_enable, OFF);
    EXPECT_EQ(tickCount, 0);
}

TEST_P(ControllerMoveForwardToStopIntegrationTest, MoveForwardUpToStopTransition_CallsMotorInterfaceCorrectly)
{
    // Given: 테스트 매개변수로부터 센서 데이터를 가져옵니다.
    SensorData sensor_data = GetParam();
    WheelState currentState = W_MOVE_FORWARD_UP;
    CleanerCommand cleaner_enable = UP;

    // When: Controller 함수를 실행합니다.
    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    // Then: 상태 전환 및 모터/클리너 명령을 확인합니다.
    EXPECT_EQ(currentState, W_STOP);
    EXPECT_EQ(lastMotorCommand, STOP); // MoveBackward(false)가 호출되어 STOP이 되어야 함
    EXPECT_EQ(cleaner_enable, OFF);
    EXPECT_EQ(tickCount, 0);
}

INSTANTIATE_TEST_SUITE_P(
    MoveForwardToStopCases,
    ControllerMoveForwardToStopIntegrationTest,
    testing::Values(
        // F=true 고정, L/R/D는 모든 경우의 수(8가지)
        ParamType{true, false, false, false}, ParamType{true, false, false, true},
        ParamType{true, false, true, false}, ParamType{true, false, true, true},
        ParamType{true, true, false, false}, ParamType{true, true, false, true},
        ParamType{true, true, true, false}, ParamType{true, true, true, true}));