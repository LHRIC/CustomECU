cmake_minimum_required(VERSION 3.16)

# Use host compiler for tests (set before project())
# Don't use CACHE to avoid contaminating the main build
if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER gcc)
endif()
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER g++)
endif()

project(CustomECU_Tests C CXX)

# C++ standard for sampling.cpp
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configure Unity before adding it
set(UNITY_EXTENSION_FIXTURE OFF CACHE BOOL "Unity fixture extension")
set(UNITY_EXTENSION_MEMORY OFF CACHE BOOL "Unity memory extension")

# Add Unity
add_subdirectory(frameworks/Unity)

# Enable double precision in Unity library
target_compile_definitions(unity PUBLIC 
    UNITY_INCLUDE_DOUBLE
    UNITY_SUPPORT_64
    UNITY_INCLUDE_FLOAT
)

# Include directories (mocks first to override real headers)
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/mocks
    ${CMAKE_SOURCE_DIR}/../Core/Inc
    frameworks/Unity/src
)

# Mock sources
set(MOCK_SOURCES
    mocks/mock_us_timer.c
    mocks/stm32f4xx_hal.c
    mocks/stm32f4xx_hal_gpio.c
    mocks/cmsis_os2.c
    mocks/ulog.c
)

# Source under test (sampling.cpp)
set(SOURCE_UNDER_TEST
    ${CMAKE_SOURCE_DIR}/../Core/Src/sampling.cpp
)

# Test 1: Sampling tests
add_executable(test_sampling
    unit/test_sampling.cpp
    ${MOCK_SOURCES}
    ${SOURCE_UNDER_TEST}
)
target_include_directories(test_sampling PRIVATE
    ${CMAKE_SOURCE_DIR}/../Core/Inc
    ${CMAKE_SOURCE_DIR}/mocks
    frameworks/Unity/src
)
target_link_libraries(test_sampling unity)
target_compile_options(test_sampling PRIVATE -DUNIT_TEST)

# Test 2: Sync detection tests
add_executable(test_sync_detection
    unit/test_sync_detection.cpp
    ${MOCK_SOURCES}
    ${SOURCE_UNDER_TEST}
    ${CMAKE_SOURCE_DIR}/../Core/Src/engine.cpp
)
target_include_directories(test_sync_detection PRIVATE
    ${CMAKE_SOURCE_DIR}/../Core/Inc
    ${CMAKE_SOURCE_DIR}/mocks
    frameworks/Unity/src
)
target_link_libraries(test_sync_detection unity)
target_compile_options(test_sync_detection PRIVATE -DUNIT_TEST)

# Add tests to CTest
enable_testing()
add_test(NAME SamplingTests COMMAND test_sampling)
add_test(NAME SyncDetectionTests COMMAND test_sync_detection)
