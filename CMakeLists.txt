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
gtest_discover_tests(test_dust)
