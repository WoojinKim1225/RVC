# CMake 버전 설정
cmake_minimum_required(VERSION 3.14)
project(MyGTestProject CXX)

# 서브모듈 경로에 있는 GTest를 빌드에 포함
add_subdirectory(third_party/googletest)

# 테스트 실행 파일 정의 및 연결은 FetchContent 방식의 2단계와 동일하게 진행합니다.
add_executable(my_unit_tests my_tests.cpp)
target_link_libraries(my_unit_tests PUBLIC gtest_main)

include(CTest)
add_test(NAME MyUnitTests_Run COMMAND my_unit_tests)