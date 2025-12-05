#include "gtest/gtest.h"
#include <stdbool.h>
#include "include/rvc.h"
// =========================================================================
// 1. 테스트 대상 함수 정의 (임시)
//
// 실제 프로젝트에서는 이 함수가 C 헤더 파일(.h)에 선언되고
// C 소스 파일(.c)에 정의되어야 합니다.
// 테스트를 위해 C++ 파일 내에 C 스타일로 임시 정의합니다.

// C++ 컴파일러가 이 함수를 C 스타일 링크 이름으로 처리하도록 지시합니다.
// extern "C"
// {
//     /**
//      * @brief 두 정수를 더하는 C 함수
//      * @param a 첫 번째 정수
//      * @param b 두 번째 정수
//      * @return 두 수의 합
//      *
//     *int Add(int a, int b)
//     {
//         return a + b;
//     }*/
//     typedef enum{
//         MOVE_FWD,
//         MOVE_BACK,
//         TURN_LEFT,
//         TURN_RIGHT,
//         STOP
//     } MotorCommand;
//     //test용 stub 구현(하드웨어 Moveforward)
    
//     typedef struct{
//         bool F;
//         bool L;
//         bool R;
//         bool D;
//     } SensorData;

//     SensorData DetermineDustExistence(bool D)
//     {
//         SensorData data = {false, false, false, D};
//         return data;
//     }
// }
// =========================================================================
extern SensorData DetermineDustExistence();
extern MotorCommand MoveForward();
extern MotorCommand MoveBackward();
extern MotorCommand TurnLeft();
extern MotorCommand TurnRight();
extern bool FrontSensorInterface(bool raw);
extern bool LeftSensorInterface(int raw);
extern bool RightSensorInterface(int raw);
// 2. GTest 테스트 케이스 정의

// TEST(TestSuiteName, TestName) 매크로를 사용하여 테스트 함수를 정의합니다.
// TestSuiteName: 테스트 그룹의 이름 (예: 기본 산술 연산)
// TestName: 해당 그룹 내의 고유한 테스트 이름 (예: 1 더하기 1)

// UT -06  DetermineDustExistenceTest: 먼지O
TEST(DetermineDustExistenceTest, DustIsTrue)
{
    
    SensorData result = DetermineDustExistence(true);
    
    EXPECT_FALSE(result.F) <<"F 값은 false";
    EXPECT_FALSE(result.L) <<"L 값은 false";
    EXPECT_FALSE(result.R) <<"R 값은 false";
    EXPECT_TRUE(result.D) <<"D 값은 입력값에 따라 true";
}
// UT -06 DetermineDustExistenceTest: 먼지X
TEST(DetermineDustExistenceTest, DustIsFalse)
{
    
    SensorData result = DetermineDustExistence(false);

    EXPECT_FALSE(result.F) <<"F 값은 false";
    EXPECT_FALSE(result.L) <<"L 값은 false";
    EXPECT_FALSE(result.R) <<"R 값은 false";
    EXPECT_FALSE(result.D) <<"D 값은 false";
}

// UT -07 MoveForwardTest: enable = true
TEST(MoveForwardTest, EnableTrue)
{
    MotorCommand cmd = MoveForward(true);
    EXPECT_EQ(MOVE_FWD, cmd);
}
// UT -07 MoveForwardTest: enable = false
TEST(MoveForwardTest, EnableFalse)
{
    MotorCommand cmd = MoveForward(false);
    EXPECT_EQ(STOP, cmd);
}
//UT -08 TurnLeftTest: turn left
TEST(TurnLeftTest, BasicTurnLeft)
{
    MotorCommand cmd = TurnLeft();
    EXPECT_EQ(TURN_LEFT, cmd);
}
//UT -09 TurnRightTest: turn right
TEST(TurnRightTest, BasicTurnRight)
{
    MotorCommand cmd = TurnRight();
    EXPECT_EQ(TURN_RIGHT, cmd);
}
//UT -10 MoveBackwardTest: eanble = true
TEST(MoveBackwardTest, EnableTrue)
{
    MotorCommand cmd = MoveBackward(true);
    EXPECT_EQ(MOVE_BACK, cmd);
}
//UT -10 MoveBackwardTest: enable = false
TEST(MoveBackwardTest, cmd)
{
    MotorCommand cmd = MoveBackward(false);
    EXPECT_EQ(STOP, cmd);
}
//IT -5_1 LeftSensorToDetermineObstacleLocation: LeftSensor = 120
TEST(LeftSensor1_IntegrationTest, LeftSensorFalse)
{
    bool L = LeftSensorInterface(120);
    SensorData data = DetermineObstacleLocation(false, L, false);
    EXPECT_FALSE(data.L);
}
//IT -5_2 LeftSensorToDetermineObstacleLocation: LeftSensor = 100
TEST(LeftSensor2_IntegrationTest, LeftSensorFalse)
{
    bool L = LeftSensorInterface(100);
    SensorData data = DetermineObstacleLocation(false, L, false);
    EXPECT_FALSE(data.L);
}
//IT -6_1 SensorInputToDetermineObstacleLocation: F:F, L:F, R:F
TEST(SensorInterface_IntegrationTest_000, FFLFRF)
{
    bool F = FrontSensorInterface(false);
    bool L = LeftSensorInterface(150);
    bool R = RightSensorInterface(150);

    SensorData data = DetermineObstacleLocation(F, L, R);

    EXPECT_FALSE(data.F);
    EXPECT_FALSE(data.L);
    EXPECT_FALSE(data.R);
}
//IT -6_2 SensorInputToDetermineObstacleLocation: F:T, L:F, R:F
TEST(SensorInterface_IntegrationTest_100, FTLFRF)
{
    bool F = FrontSensorInterface(true);
    bool L = LeftSensorInterface(150);
    bool R = RightSensorInterface(150);

    SensorData data = DetermineObstacleLocation(F, L, R);

    EXPECT_TRUE(data.F);
    EXPECT_FALSE(data.L);
    EXPECT_FALSE(data.R);
}
//IT -6_3 SensorInputToDetermineObstacleLocation: F:F, L:T, R:F
TEST(SensorInterface_IntegrationTest_010, FFLTRF)
{
    bool F = FrontSensorInterface(false);
    bool L = LeftSensorInterface(50);    // <100 → true
    bool R = RightSensorInterface(150);

    SensorData data = DetermineObstacleLocation(F, L, R);

    EXPECT_FALSE(data.F);
    EXPECT_TRUE(data.L);
    EXPECT_FALSE(data.R);
}
//IT -6_4 SensorInputToDetermineObstacleLocation: F:F, L:F, R:T
TEST(SensorInterface_IntegrationTest_001, FRLFRT)
{
    bool F = FrontSensorInterface(false);
    bool L = LeftSensorInterface(150);
    bool R = RightSensorInterface(50);   // <100 → true

    SensorData data = DetermineObstacleLocation(F, L, R);

    EXPECT_FALSE(data.F);
    EXPECT_FALSE(data.L);
    EXPECT_TRUE(data.R);
}
//IT -6_5 SensorInputToDetermineObstacleLocation: F:T, L:T, R:F
TEST(SensorInterface_IntegrationTest_110, FTLTRF)
{
    bool F = FrontSensorInterface(true);
    bool L = LeftSensorInterface(50);
    bool R = RightSensorInterface(150);

    SensorData data = DetermineObstacleLocation(F, L, R);

    EXPECT_TRUE(data.F);
    EXPECT_TRUE(data.L);
    EXPECT_FALSE(data.R);
}
//IT -6_6 SensorInputToDetermineObstacleLocation: F:T, L:F, R:T
TEST(SensorInterface_IntegrationTest_101, FTLFRT)
{
    bool F = FrontSensorInterface(true);
    bool L = LeftSensorInterface(150);
    bool R = RightSensorInterface(50);

    SensorData data = DetermineObstacleLocation(F, L, R);

    EXPECT_TRUE(data.F);
    EXPECT_FALSE(data.L);
    EXPECT_TRUE(data.R);
}
//IT -6_7 SensorInputToDetermineObstacleLocation: F:F, L:T, R:T
TEST(SensorInterface_IntegrationTest_011, FFLTRT)
{
    bool F = FrontSensorInterface(false);
    bool L = LeftSensorInterface(50);
    bool R = RightSensorInterface(50);

    SensorData data = DetermineObstacleLocation(F, L, R);

    EXPECT_FALSE(data.F);
    EXPECT_TRUE(data.L);
    EXPECT_TRUE(data.R);
}
//IT -6_8 SensorInputToDetermineObstacleLocation: F:T, L:T, R:T
TEST(SensorInterface_IntegrationTest_111, FTLTRT)
{
    bool F = FrontSensorInterface(true);
    bool L = LeftSensorInterface(50);
    bool R = RightSensorInterface(50);

    SensorData data = DetermineObstacleLocation(F, L, R);

    EXPECT_TRUE(data.F);
    EXPECT_TRUE(data.L);
    EXPECT_TRUE(data.R);
}