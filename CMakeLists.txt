# CMake 버전 설정
cmake_minimum_required(VERSION 3.14)
project(MyGTestProject LANGUAGES CXX)

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
include(CTest)
enable_testing()

# 테스트 실행 파일 정의
add_executable(my_unit_tests my_tests.cpp)

# 테스트 실행 파일을 GoogleTest와 링크
target_link_libraries(my_unit_tests PUBLIC gtest_main)

# CTest가 GoogleTest의 테스트들을 자동으로 찾도록 설정
include(GoogleTest)
gtest_discover_tests(my_unit_tests)

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
