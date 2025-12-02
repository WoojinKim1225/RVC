cmake_minimum_required(VERSION 3.14)
project(RVC1 CXX C)

# --- 프로젝트 기본 설정 ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# --- GoogleTest 설정 (FetchContent 사용) ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: GTest::gtest needs to have this property defined.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# --- 테스트 프레임워크 활성화 ---
enable_testing()
include(GoogleTest)

# ==============================================================================
# 테스트 실행 파일 정의
# ==============================================================================

# --- 테스트 1: unit_test ---
add_executable(unit_test
    moak_fun.cpp
    test/unit_test.cpp
    src/1.c
)
# 프로젝트의 'include' 디렉토리를 추가합니다.
target_include_directories(unit_test PUBLIC include)
# GTest::gtest_main을 링크하면 헤더 경로와 라이브러리가 자동으로 설정됩니다.
target_link_libraries(unit_test PRIVATE GTest::gtest_main)
gtest_discover_tests(unit_test)

# --- 테스트 2: test_wjk ---
# add_executable(test_wjk 
#     tests/test_wjk.cpp
#     src/1.c
#     tests/test_stubs.c
# )
# # 테스트 빌드 시에만 UNIT_TEST 매크로를 정의하여 스텁 함수를 활성화합니다.
# target_compile_definitions(test_wjk PRIVATE UNIT_TEST)
# target_link_libraries(test_wjk PRIVATE GTest::gtest_main)
# gtest_discover_tests(test_wjk)

# --- 테스트 3: test_dust ---
add_executable(test_dust my_tests.cpp)
target_link_libraries(test_dust PRIVATE GTest::gtest_main)
gtest_discover_tests(test_dust)
