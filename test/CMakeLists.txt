cmake_minimum_required(VERSION 3.16)

# Use host compiler for tests (set before project())
if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER gcc)
endif()

project(CustomECU_Tests C)

# Add Unity
add_subdirectory(frameworks/Unity)

# Test sources
file(GLOB TEST_SOURCES unit/*.c)

add_executable(run_tests ${TEST_SOURCES})
target_include_directories(run_tests PRIVATE frameworks/Unity/src)
target_link_libraries(run_tests unity)

add_test(NAME CustomECU_Tests COMMAND run_tests)
