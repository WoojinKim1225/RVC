#include "gtest/gtest.h"

// =========================================================================
// 1. 테스트 대상 함수 정의 (임시)
//
// 실제 프로젝트에서는 이 함수가 C 헤더 파일(.h)에 선언되고
// C 소스 파일(.c)에 정의되어야 합니다.
// 테스트를 위해 C++ 파일 내에 C 스타일로 임시 정의합니다.

// C++ 컴파일러가 이 함수를 C 스타일 링크 이름으로 처리하도록 지시합니다.
extern "C"
{
    /**
     * @brief 두 정수를 더하는 C 함수
     * @param a 첫 번째 정수
     * @param b 두 번째 정수
     * @return 두 수의 합
     */
    int Add(int a, int b)
    {
        return a + b;
    }
}
// =========================================================================

// 2. GTest 테스트 케이스 정의

// TEST(TestSuiteName, TestName) 매크로를 사용하여 테스트 함수를 정의합니다.
// TestSuiteName: 테스트 그룹의 이름 (예: 기본 산술 연산)
// TestName: 해당 그룹 내의 고유한 테스트 이름 (예: 1 더하기 1)
TEST(BasicArithmeticTest, OnePlusOneEqualsTwo)
{

    // 3. 테스트 로직

    int result = Add(1, 1);

    // GTest의 ASSERT/EXPECT 매크로를 사용하여 검증합니다.
    // EXPECT_EQ(expected, actual)는 '기대한 값'과 '실제 값'이 같은지 확인합니다.
    // 만약 실패해도 다른 테스트는 계속 진행합니다. (ASSERT_EQ는 즉시 테스트 중단)

    // 1 + 1의 결과가 2와 같은지 확인
    EXPECT_EQ(2, result) << "Add(1, 1)의 결과는 2여야 합니다.";

    // 추가적인 확인: 5 + 7의 결과가 12와 같은지 확인
    EXPECT_EQ(12, Add(5, 7));
}