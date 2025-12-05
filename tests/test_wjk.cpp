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

    extern int clean;
    extern int left;
    extern int right;
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
    SensorData sensor_data = {false, false, false, false};
    CleanerCommand cleaner_enable = UP;

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
    void SetUp() override
    {
        lastMotorCommand = MOVE_FWD;
        tickCount = 0;
    }
};

// controller에 obstacle location과 dust existence 가 들어올 때
// 출력 move forward가 motor interface에 가서 direction이 잘 나오는지
TEST_P(ControllerForwardIntegrationTest, StaysInMoveForwardState)
{
    // Given: 테스트에 할당된 센서 데이터를 가져옵니다.
    WheelState currentState = W_MOVE_FORWARD;
    SensorData sensor_data = GetParam();
    CleanerCommand cleaner_enable = ON;

    // When: Controller 함수 실행
    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    // Then: 상태 유지 및 모터 명령 확인
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD); // lastMotorCommand는 안 바뀌어야 함
    EXPECT_EQ(cleaner_enable, ON);         // cleaner도 안 바뀌어야 함
}

TEST_P(ControllerForwardIntegrationTest, MoveForwardUp_NoObstacleOrDust_CallsMotorAndCleanerInterfaceCorrectlyAfterTimeout)
{
    WheelState currentState = W_MOVE_FORWARD_UP;
    SensorData sensor_data = GetParam();
    CleanerCommand cleaner_enable = UP; // 초기 청소기 상태는 UP

    // tickCount가 0에서 4로 증가하는 동안 상태는 W_MOVE_FORWARD_UP을 유지해야 합니다.
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(tickCount, i);
        currentState = Controller(sensor_data, currentState, &cleaner_enable);
        EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
        EXPECT_EQ(lastMotorCommand, MOVE_FWD);
        EXPECT_EQ(cleaner_enable, UP);
    }

    // tickCount가 5가 되면 상태가 W_MOVE_FORWARD로 변경되어야 합니다.
    EXPECT_EQ(tickCount, 5);
    currentState = Controller(sensor_data, currentState, &cleaner_enable);
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD);
    EXPECT_EQ(cleaner_enable, ON);
    EXPECT_EQ(tickCount, 0);
}

INSTANTIATE_TEST_SUITE_P(
    MoveForwardTestCases,
    ControllerForwardIntegrationTest,
    testing::Values(
        ParamType{false, false, false, false},
        ParamType{false, true, false, false},
        ParamType{false, false, true, false},
        ParamType{false, true, true, false}));

class ControllerStopIntegrationTest : public testing::TestWithParam<ParamType>
{
protected:
    void SetUp() override
    {
        lastMotorCommand = STOP;
    }
};

TEST_P(ControllerStopIntegrationTest, StopToMoveForwardState_CallsMotorAndCleanerInterfaceCorrectly)
{
    // Given: 테스트에 할당된 센서 데이터를 가져옵니다.
    SensorData sensor_data = GetParam();
    WheelState currentState = W_STOP;
    CleanerCommand cleaner_enable = OFF;

    // When: Controller 함수 실행
    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    // Then: 상태 유지 및 모터 명령 확인
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD); // lastMotorCommand는 안 바뀌어야 함
    EXPECT_EQ(cleaner_enable, ON);         // cleaner도 안 바뀌어야 함
}

INSTANTIATE_TEST_SUITE_P(
    StopToMoveForwardState_CallsMotorAndCleanerInterfaceCorrectly,
    ControllerStopIntegrationTest,
    testing::Values(
        ParamType{false, false, false, false},
        ParamType{false, true, false, false},
        ParamType{false, false, true, false},
        ParamType{false, true, true, false}));

class ControllerStopDustIntegrationTest : public testing::TestWithParam<ParamType>
{
protected:
    void SetUp() override
    {
        lastMotorCommand = STOP;
    }
};

// W_STOP 상태에서 먼지 감지 시 W_MOVE_FORWARD_UP으로 전환되는지 테스트합니다.
TEST_P(ControllerStopDustIntegrationTest, StopToMoveForwardUpState_CallsMotorAndCleanerInterfaceCorrectly)
{
    SensorData sensor_data = GetParam();
    WheelState currentState = W_STOP;
    CleanerCommand cleaner_enable = OFF;

    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD); // MoveForward(true)가 호출되어야 함
    EXPECT_EQ(cleaner_enable, UP);         // Cleaner가 UP으로 변경되어야 함
}

INSTANTIATE_TEST_SUITE_P(
    StopToMoveForwardUpCases,
    ControllerStopDustIntegrationTest,
    testing::Values(
        ParamType{false, false, false, true},
        ParamType{false, true, false, true},
        ParamType{false, false, true, true},
        ParamType{false, true, true, true}));

TEST_F(IntegrationTest, ForwardToForwardUpState_CallsMotorAndCleanerInterfaceCorrectly)
{
    WheelState currentState = W_MOVE_FORWARD;
    SensorData sensor_data = {false, false, false, true};
    CleanerCommand cleaner_enable = ON;
    lastMotorCommand = MOVE_FWD;

    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD);
    EXPECT_EQ(cleaner_enable, UP);
}

TEST_F(IntegrationTest, MoveForwardUpToStopTransition_CallsMotorAndCleanerInterfaceCorrectly)
{
    WheelState currentState = W_MOVE_FORWARD_UP;
    SensorData sensor_data = {true, false, false, false};
    CleanerCommand cleaner_enable = UP;
    lastMotorCommand = MOVE_FWD;

    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    EXPECT_EQ(currentState, W_STOP);
    EXPECT_EQ(lastMotorCommand, STOP);
    EXPECT_EQ(cleaner_enable, OFF);
    EXPECT_EQ(tickCount, 0);
}

TEST_F(IntegrationTest, StopToTurnRightTransition_CallsMotorAndCleanerInterfaceCorrectly)
{
    WheelState currentState = W_STOP;
    SensorData sensor_data = {true, true, false, false};
    CleanerCommand cleaner_enable = OFF;
    lastMotorCommand = STOP;

    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    EXPECT_EQ(currentState, W_TURN_RIGHT);
    EXPECT_EQ(lastMotorCommand, TURN_RIGHT);
    EXPECT_EQ(cleaner_enable, OFF);
    EXPECT_EQ(tickCount, 0);
}

TEST_F(IntegrationTest, StopToTurnLeftTransition_CallsMotorAndCleanerInterfaceCorrectly)
{
    WheelState currentState = W_STOP;
    SensorData sensor_data = {true, false, true, false};
    CleanerCommand cleaner_enable = OFF;
    lastMotorCommand = STOP;

    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    EXPECT_EQ(currentState, W_TURN_LEFT);
    EXPECT_EQ(lastMotorCommand, TURN_LEFT);
    EXPECT_EQ(cleaner_enable, OFF);
    EXPECT_EQ(tickCount, 0);
}

class ControllerMoveForwardToStopIntegrationTest : public testing::TestWithParam<ParamType>
{
protected:
    void SetUp() override
    {
        lastMotorCommand = MOVE_FWD;
        tickCount = 0;
    }
};

// W_MOVE_FORWARD 상태에서 전방 장애물 감지 시 W_STOP으로 전환되는지 테스트합니다.
// L, R, D 센서 값에 관계없이 동일하게 동작해야 합니다.
TEST_P(ControllerMoveForwardToStopIntegrationTest, MoveForwardToStopTransition_CallsMotorAndCleanerInterfaceCorrectly)
{
    SensorData sensor_data = GetParam();
    WheelState currentState = W_MOVE_FORWARD;
    CleanerCommand cleaner_enable = ON;

    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    EXPECT_EQ(currentState, W_STOP);
    EXPECT_EQ(lastMotorCommand, STOP);
    EXPECT_EQ(cleaner_enable, OFF);
    EXPECT_EQ(tickCount, 0);
}

TEST_P(ControllerMoveForwardToStopIntegrationTest, MoveForwardUpToStopTransition_CallsMotorAndCleanerInterfaceCorrectly)
{
    SensorData sensor_data = GetParam();
    WheelState currentState = W_MOVE_FORWARD_UP;
    CleanerCommand cleaner_enable = UP;

    currentState = Controller(sensor_data, currentState, &cleaner_enable);

    EXPECT_EQ(currentState, W_STOP);
    EXPECT_EQ(lastMotorCommand, STOP);
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

//=========================================SYSTEM================================================

class SystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        tickCount = 0;
    }
};

// 시나리오: Power-up 전진 중(W_MOVE_FORWARD_UP), 전방 장애물 감지 시
// 기대 동작: 정지(W_STOP), 모터 정지(STOP), 클리너 OFF
TEST_F(SystemTest, MoveForwardUp_To_Stop_WhenFrontObstacle)
{
    // Given: 초기 상태 및 센서 값 설정
    stub_front_input = true; // 전방 장애물
    stub_left_input = 50;    // 좌측 장애물 있음
    stub_right_input = 50;   // 우측 장애물 있음
    stub_dust_input = 800;   // 먼지 있음
    WheelState currentState = W_MOVE_FORWARD_UP;
    CleanerCommand cleaner_enable = UP;
    lastMotorCommand = MOVE_FWD; // 초기 모터 상태
    clean = 127;
    left = 127;
    right = 127;

    // When: 시스템 로직 실행
    SensorData merged_data = Merge_Sensordata(
        DetermineObstacleLocation(FrontSensorInterface(ReadFrontSensor()), LeftSensorInterface(ReadLeftSensor()), RightSensorInterface(ReadRightSensor())),
        DetermineDustExistence(DustSensorInterface(ReadDustSensor())));
    currentState = Controller(merged_data, currentState, &cleaner_enable);
    Cleaner(cleaner_enable);

    // Then: 결과 검증
    EXPECT_EQ(currentState, W_STOP);
    EXPECT_EQ(lastMotorCommand, STOP);
    EXPECT_EQ(lastCleanerCommand, OFF);

    EXPECT_EQ(clean, 0);
    EXPECT_EQ(left, 0);
    EXPECT_EQ(right, 0);
}

// 시나리오: 정지 중(W_STOP), 전방과 좌측 장애물, 먼지 감지 시
// 기대 동작: 우회전(W_TURN_RIGHT), 모터 우회전(TURN_RIGHT), 클리너 OFF
TEST_F(SystemTest, Stop_To_TurnRight_WhenFrontLeftObstacle)
{
    // Given: 초기 상태 및 센서 값 설정
    stub_front_input = true; // 전방 장애물
    stub_left_input = 50;    // 좌측 장애물
    stub_right_input = 200;  // 우측 장애물 없음
    stub_dust_input = 800;   // 먼지 있음
    WheelState currentState = W_STOP;
    CleanerCommand cleaner_enable = OFF;
    lastMotorCommand = STOP;
    clean = 0;
    left = 0;
    right = 0;

    // When: 시스템 로직 실행
    SensorData merged_data = Merge_Sensordata(
        DetermineObstacleLocation(FrontSensorInterface(ReadFrontSensor()), LeftSensorInterface(ReadLeftSensor()), RightSensorInterface(ReadRightSensor())),
        DetermineDustExistence(DustSensorInterface(ReadDustSensor())));
    currentState = Controller(merged_data, currentState, &cleaner_enable);
    Cleaner(cleaner_enable);

    // Then: 결과 검증
    EXPECT_EQ(currentState, W_TURN_RIGHT);
    EXPECT_EQ(lastMotorCommand, TURN_RIGHT);
    EXPECT_EQ(lastCleanerCommand, OFF);

    EXPECT_EQ(clean, 0);
    EXPECT_EQ(left, -127);
    EXPECT_EQ(right, 127);
}

// 시나리오: 일반 전진 중(W_MOVE_FORWARD), 우측 장애물과 먼지 감지 시
// 기대 동작: 상태 유지(W_MOVE_FORWARD), 모터 전진(MOVE_FWD), 클리너 Power Up
TEST_F(SystemTest, MoveForward_Stays_WhenRightObstacleAndDust)
{
    // Given: 초기 상태 및 센서 값 설정
    stub_front_input = false; // 전방 장애물 없음
    stub_left_input = 200;    // 좌측 장애물 없음
    stub_right_input = 50;    // 우측 장애물
    stub_dust_input = 800;    // 먼지 있음
    WheelState currentState = W_MOVE_FORWARD;
    CleanerCommand cleaner_enable = ON;
    lastMotorCommand = MOVE_FWD;
    clean = 0;
    left = 127;
    right = 127;

    // When: 시스템 로직 실행
    SensorData merged_data = Merge_Sensordata(
        DetermineObstacleLocation(FrontSensorInterface(ReadFrontSensor()), LeftSensorInterface(ReadLeftSensor()), RightSensorInterface(ReadRightSensor())),
        DetermineDustExistence(DustSensorInterface(ReadDustSensor())));
    currentState = Controller(merged_data, currentState, &cleaner_enable);
    Cleaner(cleaner_enable);

    // Then: 결과 검증
    EXPECT_EQ(currentState, W_MOVE_FORWARD_UP); // 먼지 감지로 UP 상태로 변경
    EXPECT_EQ(lastMotorCommand, MOVE_FWD);      // 모터는 계속 전진
    EXPECT_EQ(cleaner_enable, UP);
    EXPECT_EQ(clean, 127);
    EXPECT_EQ(left, 127);
    EXPECT_EQ(right, 127);
}

// 시나리오: Power-up 전진 중(W_MOVE_FORWARD_UP), 전방 장애물과 먼지 감지 시
// 기대 동작: 정지(W_STOP), 모터 정지(STOP), 클리너 OFF
TEST_F(SystemTest, MoveForwardUp_To_Stop_WhenFrontObstacleAndDust)
{
    // Given: 초기 상태 및 센서 값 설정
    stub_front_input = true; // 전방 장애물
    stub_left_input = 200;   // 좌측 장애물 없음
    stub_right_input = 200;  // 우측 장애물 없음
    stub_dust_input = 800;   // 먼지 있음
    WheelState currentState = W_MOVE_FORWARD_UP;
    CleanerCommand cleaner_enable = UP;
    lastMotorCommand = MOVE_FWD;
    clean = 127;
    left = 127;
    right = 127;

    // When: 시스템 로직 실행
    SensorData merged_data = Merge_Sensordata(
        DetermineObstacleLocation(FrontSensorInterface(ReadFrontSensor()), LeftSensorInterface(ReadLeftSensor()), RightSensorInterface(ReadRightSensor())),
        DetermineDustExistence(DustSensorInterface(ReadDustSensor())));
    currentState = Controller(merged_data, currentState, &cleaner_enable);
    Cleaner(cleaner_enable);

    // Then: 결과 검증
    EXPECT_EQ(currentState, W_STOP);
    EXPECT_EQ(lastMotorCommand, STOP);
    EXPECT_EQ(cleaner_enable, OFF);

    EXPECT_EQ(clean, 0);
    EXPECT_EQ(left, 0);
    EXPECT_EQ(right, 0);
}

// 시나리오: Power-up 전진 중(tickCount=4), 좌/우 장애물과 먼지 감지 시
// 기대 동작: 상태 유지(W_MOVE_FORWARD_UP), 5틱 후 일반 전진(W_MOVE_FORWARD)으로 전환
TEST_F(SystemTest, MoveForwardUp_TransitionsToMoveForward_AfterTimeout)
{
    // Given: 초기 상태 및 센서 값 설정
    stub_front_input = false; // 전방 장애물 없음
    stub_left_input = 50;     // 좌측 장애물
    stub_right_input = 50;    // 우측 장애물
    stub_dust_input = 800;    // 먼지 있음
    WheelState currentState = W_MOVE_FORWARD_UP;
    CleanerCommand cleaner_enable = UP;
    lastMotorCommand = MOVE_FWD;
    tickCount = 4; // tickCount를 4로 설정
    clean = 127;
    left = 127;
    right = 127;

    // When: 시스템 로직 실행 (tick 4)
    SensorData merged_data = Merge_Sensordata(
        DetermineObstacleLocation(FrontSensorInterface(ReadFrontSensor()), LeftSensorInterface(ReadLeftSensor()), RightSensorInterface(ReadRightSensor())),
        DetermineDustExistence(DustSensorInterface(ReadDustSensor())));
    currentState = Controller(merged_data, currentState, &cleaner_enable);
    Cleaner(cleaner_enable);

    // Then: tick 4에서는 상태 유지
    EXPECT_EQ(currentState, W_MOVE_FORWARD_UP);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD);
    EXPECT_EQ(cleaner_enable, UP);
    EXPECT_EQ(tickCount, 5);
    EXPECT_EQ(clean, 127);
    EXPECT_EQ(left, 127);
    EXPECT_EQ(right, 127);

    // When: 시스템 로직 실행 (tick 5)
    // 센서 값 변경 없음 (먼지 사라짐)
    stub_dust_input = 200;
    merged_data = Merge_Sensordata(
        DetermineObstacleLocation(FrontSensorInterface(ReadFrontSensor()), LeftSensorInterface(ReadLeftSensor()), RightSensorInterface(ReadRightSensor())),
        DetermineDustExistence(DustSensorInterface(ReadDustSensor())));
    currentState = Controller(merged_data, currentState, &cleaner_enable);
    Cleaner(cleaner_enable);

    // Then: tick 5에서 상태 변경
    EXPECT_EQ(currentState, W_MOVE_FORWARD);
    EXPECT_EQ(lastMotorCommand, MOVE_FWD);
    EXPECT_EQ(lastCleanerCommand, ON);
    EXPECT_EQ(tickCount, 0);
    EXPECT_EQ(clean, 64);
    EXPECT_EQ(left, 127);
    EXPECT_EQ(right, 127);
}