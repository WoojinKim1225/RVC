cmake_minimum_required(VERSION 3.14)
project(RVC1 CXX C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# GoogleTest 다운로드 & 빌드
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# --- CTest 설정 ---
# include(CTest)
# enable_testing()

# 테스트 실행 파일 정의
add_executable(unit_test
    moak_fun.cpp
    test/unit_test.cpp
    src/1.c
)

# include 디렉토리 연결
target_include_directories(unit_test PUBLIC include)

# 테스트 실행 파일을 GoogleTest와 링크
target_link_libraries(unit_test
enable_testing()

# 테스트 소스
add_executable(test_dust
    my_tests.cpp
)

# 여기가 핵심!
target_link_libraries(test_dust
    gtest
    gtest_main
    pthread
)

# 여기 추가하면 include 경로 자동 연결됨 (너에게 가장 중요한 줄)
target_include_directories(test_dust PUBLIC
    ${gtest_SOURCE_DIR}/include
    ${gmock_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(unit_test)

# testwjk 테스트 추가 (testwjk.cpp 소스 파일 필요)
# add_executable(<실행파일_이름> [소스1] [소스2] ...)
# 테스트 대상 코드(2.c)와 테스트용 스텁(test_stubs.c)을 함께 컴파일합니다.
add_executable(test_wjk 
    tests/test_wjk.cpp
    src/2.c
    tests/test_stubs.c)
# 테스트 빌드 시에만 UNIT_TEST 매크로를 정의합니다.
target_compile_definitions(test_wjk PRIVATE UNIT_TEST)
target_link_libraries(test_wjk PUBLIC gtest_main)
gtest_discover_tests(test_wjk)
gtest_discover_tests(test_dust)
