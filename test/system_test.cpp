#include <gtest/gtest.h>
#include "rvc.h"

extern "C"
{
    /* mock/stub 변수 가져오기 */
    bool stub_front_input;
    int stub_left_input;
    int stub_right_input;
    int stub_dust_input;
    WheelState stub_wheelstate;
    MotorCommand lastMotorCommand;
    CleanerCommand lastCleanerCommand;

    int clean;
    int left;
    int right;
}
// typedef enum {
//     MOVE_FWD,
//     MOVE_BACK,
//     TURN_LEFT,
//     TURN_RIGHT,
//     STOP
// } MotorCommand;

// /* Cleaner actuator command enum */
// typedef enum {
//     OFF,
//     ON,
//     UP
// } CleanerCommand;

// /* WheelState: FSM states */
// typedef enum {
//     W_STOP,
//     W_MOVE_FORWARD,
//     W_MOVE_BACK,
//     W_TURN_LEFT,
//     W_TURN_RIGHT,
//     W_MOVE_FORWARD_UP
// } WheelState;

// class system_test : public ::testing::Test {
// protected:
//     void SetUp() override {
//         tickCount = 0;
//         lastMotorCommand = MOVE_BACK;
//         lastCleanerCommand = OFF;
//     }
// };
TEST(system_test, systemtest1)
{
    tickCount = 0;
    lastMotorCommand = MOVE_BACK;
    lastCleanerCommand = OFF;
    stub_front_input = false;
    stub_left_input = 120;
    stub_right_input = 80;
    stub_dust_input = 700;
    stub_wheelstate = W_MOVE_BACK;

    bool F = FrontSensorInterface(stub_front_input);
    bool L = LeftSensorInterface(stub_left_input);
    bool R = RightSensorInterface(stub_right_input);
    bool D = DustSensorInterface(stub_dust_input);

    SensorData obstacle_Location = DetermineObstacleLocation(F, L, R);
    SensorData dust_Existence = DetermineDustExistence(D);

    /* Step 2: Merge obstacle + dust info */
    SensorData merged_data = Merge_Sensordata(obstacle_Location, dust_Existence);

    /* Step 3: Compute actuator commands */
    stub_wheelstate = Controller(merged_data, stub_wheelstate, &lastCleanerCommand);
    Cleaner(lastCleanerCommand);
    Motor(lastMotorCommand);

    EXPECT_EQ(clean, 0);
    // ✔ Motor 결과 확인 (왼쪽/오른쪽 휠)
    EXPECT_EQ(left, -127);
    EXPECT_EQ(right, 127);
}
TEST(system_test, systemtest2)
{
    tickCount = 2;
    lastMotorCommand = MOVE_FWD;
    lastCleanerCommand = UP;
    stub_front_input = true;
    stub_left_input = 67;
    stub_right_input = 80;
    stub_dust_input = 300;
    stub_wheelstate = W_MOVE_FORWARD_UP;

    bool F = FrontSensorInterface(stub_front_input);
    bool L = LeftSensorInterface(stub_left_input);
    bool R = RightSensorInterface(stub_right_input);
    bool D = DustSensorInterface(stub_dust_input);

    SensorData obstacle_Location = DetermineObstacleLocation(F, L, R);
    SensorData dust_Existence = DetermineDustExistence(D);

    /* Step 2: Merge obstacle + dust info */
    SensorData merged_data = Merge_Sensordata(obstacle_Location, dust_Existence);

    /* Step 3: Compute actuator commands */
    stub_wheelstate = Controller(merged_data, stub_wheelstate, &lastCleanerCommand);
    Cleaner(lastCleanerCommand);
    Motor(lastMotorCommand);

    EXPECT_EQ(clean, 0);
    // ✔ Motor 결과 확인 (왼쪽/오른쪽 휠)
    EXPECT_EQ(left, 0);
    EXPECT_EQ(right, 0);
}
TEST(system_test, systemtest3)
{
    tickCount = 3;
    lastMotorCommand = MOVE_FWD;
    lastCleanerCommand = UP;
    stub_front_input = true;
    stub_left_input = 58;
    stub_right_input = 130;
    stub_dust_input = 400;
    stub_wheelstate = W_MOVE_FORWARD_UP;

    bool F = FrontSensorInterface(stub_front_input);
    bool L = LeftSensorInterface(stub_left_input);
    bool R = RightSensorInterface(stub_right_input);
    bool D = DustSensorInterface(stub_dust_input);

    SensorData obstacle_Location = DetermineObstacleLocation(F, L, R);
    SensorData dust_Existence = DetermineDustExistence(D);

    /* Step 2: Merge obstacle + dust info */
    SensorData merged_data = Merge_Sensordata(obstacle_Location, dust_Existence);

    /* Step 3: Compute actuator commands */
    stub_wheelstate = Controller(merged_data, stub_wheelstate, &lastCleanerCommand);
    Cleaner(lastCleanerCommand);
    Motor(lastMotorCommand);

    EXPECT_EQ(clean, 0);
    // ✔ Motor 결과 확인 (왼쪽/오른쪽 휠)
    EXPECT_EQ(left, 0);
    EXPECT_EQ(right, 0);
}
TEST(system_test, systemtest4)
{
    tickCount = 0;
    lastMotorCommand = MOVE_FWD;
    lastCleanerCommand = ON;
    stub_front_input = false;
    stub_left_input = 180;
    stub_right_input = 30;
    stub_dust_input = 400;
    stub_wheelstate = W_MOVE_FORWARD;

    bool F = FrontSensorInterface(stub_front_input);
    bool L = LeftSensorInterface(stub_left_input);
    bool R = RightSensorInterface(stub_right_input);
    bool D = DustSensorInterface(stub_dust_input);

    SensorData obstacle_Location = DetermineObstacleLocation(F, L, R);
    SensorData dust_Existence = DetermineDustExistence(D);

    /* Step 2: Merge obstacle + dust info */
    SensorData merged_data = Merge_Sensordata(obstacle_Location, dust_Existence);

    /* Step 3: Compute actuator commands */
    stub_wheelstate = Controller(merged_data, stub_wheelstate, &lastCleanerCommand);
    Cleaner(lastCleanerCommand);
    Motor(lastMotorCommand);

    EXPECT_EQ(clean, 64);
    // ✔ Motor 결과 확인 (왼쪽/오른쪽 휠)
    EXPECT_EQ(left, 127);
    EXPECT_EQ(right, 127);
}
TEST(system_test, systemtest5)
{
    tickCount = 0;
    lastMotorCommand = STOP;
    lastCleanerCommand = OFF;
    stub_front_input = true;
    stub_left_input = 180;
    stub_right_input = 300;
    stub_dust_input = 400;
    stub_wheelstate = W_STOP;

    bool F = FrontSensorInterface(stub_front_input);
    bool L = LeftSensorInterface(stub_left_input);
    bool R = RightSensorInterface(stub_right_input);
    bool D = DustSensorInterface(stub_dust_input);

    SensorData obstacle_Location = DetermineObstacleLocation(F, L, R);
    SensorData dust_Existence = DetermineDustExistence(D);

    /* Step 2: Merge obstacle + dust info */
    SensorData merged_data = Merge_Sensordata(obstacle_Location, dust_Existence);

    /* Step 3: Compute actuator commands */
    stub_wheelstate = Controller(merged_data, stub_wheelstate, &lastCleanerCommand);
    Cleaner(lastCleanerCommand);
    Motor(lastMotorCommand);

    EXPECT_EQ(clean, 0);
    // ✔ Motor 결과 확인 (왼쪽/오른쪽 휠)
    EXPECT_EQ(left, -127);
    EXPECT_EQ(right, 127);
}
TEST(system_test, systemtest6)
{
    tickCount = 0;
    lastMotorCommand = MOVE_BACK;
    lastCleanerCommand = OFF;
    stub_front_input = false;
    stub_left_input = 90;
    stub_right_input = 70;
    stub_dust_input = 400;
    stub_wheelstate = W_MOVE_BACK;

    bool F = FrontSensorInterface(stub_front_input);
    bool L = LeftSensorInterface(stub_left_input);
    bool R = RightSensorInterface(stub_right_input);
    bool D = DustSensorInterface(stub_dust_input);

    SensorData obstacle_Location = DetermineObstacleLocation(F, L, R);
    SensorData dust_Existence = DetermineDustExistence(D);

    /* Step 2: Merge obstacle + dust info */
    SensorData merged_data = Merge_Sensordata(obstacle_Location, dust_Existence);

    /* Step 3: Compute actuator commands */
    stub_wheelstate = Controller(merged_data, stub_wheelstate, &lastCleanerCommand);
    Cleaner(lastCleanerCommand);
    Motor(lastMotorCommand);

    EXPECT_EQ(clean, 0);
    // ✔ Motor 결과 확인 (왼쪽/오른쪽 휠)
    EXPECT_EQ(left, -127);
    EXPECT_EQ(right, -127);
}