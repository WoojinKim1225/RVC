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


# --- 공통 스텁 라이브러리 정의 ---
# 모든 테스트에서 사용할 스텁 함수들을 모아 정적 라이브러리를 생성합니다.
add_library(test_stubs STATIC
    moak_fun.cpp
    tests/test_stubs.c
)
# 스텁 라이브러리가 'include' 디렉토리를 참조하도록 설정합니다.
target_include_directories(test_stubs PUBLIC include)

# --- 테스트 1: unit_test ---
add_executable(unit_test
    test/unit_test.cpp
    src/1.c
)
target_compile_definitions(unit_test PRIVATE UNIT_TEST) # UNIT_TEST 매크로 정의
target_include_directories(unit_test PUBLIC include) # 테스트 파일이 헤더를 찾을 수 있도록 경로 추가
target_link_libraries(unit_test PRIVATE GTest::gtest_main test_stubs)
gtest_discover_tests(unit_test)

# --- 테스트 2: test_wjk ---
add_executable(test_wjk 
    src/1.c
    tests/test_wjk.cpp
)
target_compile_definitions(test_wjk PRIVATE UNIT_TEST) # UNIT_TEST 매크로 정의
target_include_directories(test_wjk PUBLIC include) # 테스트 파일이 헤더를 찾을 수 있도록 경로 추가
target_link_libraries(test_wjk PRIVATE GTest::gtest_main test_stubs)
gtest_discover_tests(test_wjk)

# --- 테스트 3: test_dust ---
add_executable(test_dust 
    my_tests.cpp
    src/1.c
)
target_compile_definitions(test_dust PRIVATE UNIT_TEST) # UNIT_TEST 매크로 정의
target_include_directories(test_dust PUBLIC include) # 테스트 파일이 헤더를 찾을 수 있도록 경로 추가
target_link_libraries(test_dust PRIVATE GTest::gtest_main test_stubs)
gtest_discover_tests(test_dust)

# --- 테스트 4: integration test ---
add_executable(inte_test 
test/integration_test.cpp
src/1.c
)
target_compile_definitions(inte_test PRIVATE UNIT_TEST)
target_include_directories(inte_test PUBLIC include)
target_link_libraries(inte_test PRIVATE GTest::gtest_main test_stubs)
gtest_discover_tests(inte_test)

# -- 테스트 5: system test
add_executable(system_tests
test/system_test.cpp
src/1.c
)
target_compile_definitions(system_tests PRIVATE UNIT_TEST)
target_include_directories(system_tests PUBLIC include)
target_link_libraries(system_tests PRIVATE GTest::gtest_main test_stubs)
gtest_discover_tests(system_tests)