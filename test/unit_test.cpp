#include <gtest/gtest.h>
#include "rvc.h"

/* mock/stub 변수 가져오기 */
extern bool stub_front_input;
extern int stub_left_input;
extern int stub_right_input;
extern int stub_dust_input;

/* --------------------------------------------------------- */
/* 1. FrontSensorInterface 테스트                             */
/* --------------------------------------------------------- */
TEST(Unit_SensorInterface, FrontSensor_ReturnsCorrectBinary) {
    // front sensor = true
    stub_front_input = true;
    EXPECT_TRUE( FrontSensorInterface(ReadFrontSensor()) );

    // front sensor = false
    stub_front_input = false;
    EXPECT_FALSE( FrontSensorInterface(ReadFrontSensor()) );
}

/* --------------------------------------------------------- */
/* 2. LeftSensorInterface 테스트                              */
/* --------------------------------------------------------- */
TEST(Unit_SensorInterface, LeftSensor_ReturnsObstacleIfLessThan100) {
    stub_left_input = 50;   // obstacle
    EXPECT_TRUE( LeftSensorInterface(ReadLeftSensor()) );

    stub_left_input = 150;  // no obstacle
    EXPECT_FALSE( LeftSensorInterface(ReadLeftSensor()) );

    stub_left_input = 100; // 기준점
    EXPECT_FALSE( LeftSensorInterface(ReadLeftSensor()) );

}

/* --------------------------------------------------------- */
/* 3. RightSensorInterface 테스트                             */
/* --------------------------------------------------------- */
TEST(Unit_SensorInterface, RightSensor_ReturnsObstacleIfLessThan100) {
    stub_right_input = 80;   // obstacle
    EXPECT_TRUE( RightSensorInterface(ReadRightSensor()) );

    stub_right_input = 140;  // no obstacle
    EXPECT_FALSE( RightSensorInterface(ReadRightSensor()) );

    stub_right_input = 100; // 기준점
    EXPECT_FALSE( RightSensorInterface(ReadLeftSensor()) );
}

/* --------------------------------------------------------- */
/* 4. DustSensorInterface 테스트                              */
/* --------------------------------------------------------- */
TEST(Unit_SensorInterface, DustSensor_ReturnsTrueIfGreaterThan600) {
    stub_dust_input = 700;  // dust detected
    EXPECT_TRUE( DustSensorInterface(ReadDustSensor()) );

    stub_dust_input = 400;  // no dust
    EXPECT_FALSE( DustSensorInterface(ReadDustSensor()) );

    stub_dust_input = 600;  // 기준점
    EXPECT_FALSE( DustSensorInterface(ReadDustSensor()) );
}

/* --------------------------------------------------------- */
/* 5. DetermineObstacleLocation 테스트                         */
/* --------------------------------------------------------- */
TEST(Unit_DetermineObstacleLocation, CorrectObstacleMapping) {
    SensorData data = DetermineObstacleLocation(true, false, true);

    EXPECT_TRUE(data.F);
    EXPECT_FALSE(data.L);
    EXPECT_TRUE(data.R);
    EXPECT_FALSE(data.D);   // dust always false in obstacle function

    SensorData data1 = DetermineObstacleLocation(false, false, false);

    EXPECT_FALSE(data1.F);
    EXPECT_FALSE(data1.L);
    EXPECT_FALSE(data1.R);
    EXPECT_FALSE(data1.D);
  
}