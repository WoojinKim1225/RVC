# CMake 버전 설정
cmake_minimum_required(VERSION 3.14)
project(MyGTestProject LANGUAGES C CXX)

# --- GoogleTest 통합 (FetchContent 사용) ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# --- CTest 설정 ---
# include(CTest)
# enable_testing()

# 테스트 실행 파일 정의
add_executable(unit_test
    unit_test.cpp
    moak_fun.cpp
    src/1.c
)

# include 디렉토리 연결
target_include_directories(unit_test PUBLIC include)

# 테스트 실행 파일을 GoogleTest와 링크
target_link_libraries(unit_test
    gtest
    gtest_main
    pthread
)

# CTest가 GoogleTest의 테스트들을 자동으로 찾도록 설정
include(GoogleTest)
gtest_discover_tests(unit_test)

