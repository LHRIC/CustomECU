# Unit Testing Setup for CustomECU

This project uses Unity test framework for unit testing the synchronization components.

## Directory Structure

```
test/
├── CMakeLists.txt          # Test build configuration
├── frameworks/             # Test frameworks (submodules)
│   ├── Unity/             # Unity test framework
│   └── CMock/             # CMock mocking framework
├── mocks/                 # Mock implementations for embedded dependencies
│   ├── cmsis_os2.h/c      # CMSIS-RTOS2 mocks
│   ├── stm32f4xx_hal.h/c  # STM32 HAL mocks
│   ├── mock_us_timer.h/c  # Microsecond timer mocks
│   ├── mock_main.h/c      # Main header mocks
│   └── ulog.h/c           # Logging system mocks
└── unit/                  # Unit test files
    ├── test_sampling.cpp          # Tests for sampling.cpp
    └── test_sync_detection.cpp    # Tests for sync detection logic
```

## Building and Running Tests

### Quick Start

From the project root directory:

```bash
./run_tests.sh
```

### Manual Build

```bash
cd test
mkdir -p build
cd build
cmake ..
make
ctest --verbose
```

Or run individual test executables:

```bash
./test_sampling
./test_sync_detection
```

## Test Coverage

### test_sampling.cpp
Tests the core sampling functionality:
- Initial state validation
- Crank tooth detection and counter increment
- Tooth period calculation with exponential moving average
- Cam tooth detection and counter tracking
- Real-time fraction of tooth calculation
- Real-time engine angle calculation
- Crank index rollover behavior
- Engine phase transitions

### test_sync_detection.cpp
Tests the synchronization detection logic:
- Sync detection with valid cam deltas (1, 11, 12 teeth)
- Sync rejection with invalid cam deltas
- Full synchronization sequence
- Sync loss and re-establishment
- Edge cases and counter overflow protection

## Mock Strategy

The test environment mocks all embedded dependencies:

1. **HAL Layer**: STM32 Hardware Abstraction Layer functions
2. **CMSIS-RTOS**: FreeRTOS CMSIS wrapper functions
3. **Timers**: Microsecond timer with controllable time
4. **Logging**: ULOG macros redirect to printf for test visibility

### Controllable Mock Timer

The `mock_us_timer` provides full control over time in tests:

```cpp
// Set specific time
mock_us_timer_set_time(1000);

// Advance time
mock_us_timer_advance(500);

// Reset to zero
mock_us_timer_reset();
```

This allows deterministic testing of time-dependent behavior.

## Adding New Tests

1. Create a new test file in `test/unit/`:

```cpp
#include "unity.h"
#include "mock_us_timer.h"
#include "sampling.hpp"

extern "C" void setUp(void) {
    // Reset state before each test
    syncState.synced = false;
    // ... reset other fields
}

extern "C" void tearDown(void) {
    // Cleanup after each test
}

extern "C" void test_my_new_feature(void) {
    // Arrange
    mock_us_timer_set_time(1000);
    
    // Act
    on_crank_tooth();
    
    // Assert
    TEST_ASSERT_EQUAL_UINT64(1, syncState.crank_counter);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_my_new_feature);
    return UNITY_END();
}
```

2. Add the test executable to `test/CMakeLists.txt`:

```cmake
add_executable(test_my_feature
    unit/test_my_feature.cpp
    ${MOCK_SOURCES}
    ${SOURCE_UNDER_TEST}
)
target_include_directories(test_my_feature PRIVATE
    ${CMAKE_SOURCE_DIR}/../Core/Inc
    ${CMAKE_SOURCE_DIR}/mocks
    frameworks/Unity/src
)
target_link_libraries(test_my_feature unity)
target_compile_options(test_my_feature PRIVATE -DUNIT_TEST)

add_test(NAME MyFeatureTests COMMAND test_my_feature)
```

3. Rebuild and run tests

## Continuous Integration

The tests are designed to run on any x86/x64 system with gcc/g++, making them suitable for CI/CD pipelines.

## Troubleshooting

### Build Errors

If you see CMake cache errors:
```bash
cd test/build
rm -rf *
cmake ..
make
```

### Missing Submodules

If Unity or CMock are not found:
```bash
git submodule update --init --recursive
```

### Test Failures

Run tests with verbose output:
```bash
cd test/build
ctest --verbose
```

Or run individual tests directly to see full output:
```bash
./test_sampling
```
