#include "mock_main.h"
#include "mock_us_timer.h"
#include "unity.h"

// Mock ULOG macros for testing
#define ULOG_DEBUG(...)
#define ULOG_INFO(...)
#define ULOG_ERROR(...)

// Include sampling header
#include "sampling.hpp"

// Declare the function we're testing
void detectSync(void);

void setUp(void) {
  // Reset sync state before each test
  syncState.synced = false;
  syncState.crank_index = 0;
  syncState.crank_counter = 0;
  syncState.cam_crank_counter = 0;
  syncState.last_cam_crank_counter = 0;
  syncState.last_crank_time_us = 0;
  syncState.last_cam_time_us = 0;
  syncState.tooth_period_us = 0.0;
  syncState.engine_phase = false;
}

void tearDown(void) {
  // Cleanup
}

// ============================================================================
// Test: Sync Detection with Delta = 2
// ============================================================================
void test_detect_sync_with_delta_2_sets_correct_state(void) {
  // Setup: cam teeth with delta of 2 crank teeth
  syncState.cam_crank_counter = 5;
  syncState.last_cam_crank_counter = 3; // Delta = 2

  detectSync();

  TEST_ASSERT_TRUE(syncState.synced);
  TEST_ASSERT_EQUAL_UINT8(1, syncState.crank_index);
  TEST_ASSERT_TRUE(syncState.engine_phase); // Phase 1
}

// ============================================================================
// Test: Sync Detection with Delta = 10
// ============================================================================
void test_detect_sync_with_delta_10_sets_correct_state(void) {
  // Setup: cam teeth with delta of 10 crank teeth
  syncState.cam_crank_counter = 20;
  syncState.last_cam_crank_counter = 10; // Delta = 10

  detectSync();

  TEST_ASSERT_TRUE(syncState.synced);
  TEST_ASSERT_EQUAL_UINT8(10, syncState.crank_index);
  TEST_ASSERT_FALSE(syncState.engine_phase); // Phase 0
}

// ============================================================================
// Test: Sync Detection with Delta = 12
// ============================================================================
void test_detect_sync_with_delta_12_sets_correct_state(void) {
  // Setup: cam teeth with delta of 12 crank teeth
  syncState.cam_crank_counter = 25;
  syncState.last_cam_crank_counter = 13; // Delta = 12

  detectSync();

  TEST_ASSERT_TRUE(syncState.synced);
  TEST_ASSERT_EQUAL_UINT8(0, syncState.crank_index);
  TEST_ASSERT_FALSE(syncState.engine_phase); // Phase 0
}

// ============================================================================
// Test: Sync Detection with Invalid Delta
// ============================================================================
void test_detect_sync_with_invalid_delta_stays_not_synced(void) {
  // Setup: cam teeth with invalid delta
  syncState.cam_crank_counter = 10;
  syncState.last_cam_crank_counter = 5; // Delta = 5 (invalid)

  detectSync();

  TEST_ASSERT_FALSE(syncState.synced);
}

void test_detect_sync_with_delta_0_stays_not_synced(void) {
  // Setup: no change in cam counter
  syncState.cam_crank_counter = 10;
  syncState.last_cam_crank_counter = 10; // Delta = 0

  detectSync();

  TEST_ASSERT_FALSE(syncState.synced);
}

void test_detect_sync_with_delta_13_stays_not_synced(void) {
  // Setup: delta too large
  syncState.cam_crank_counter = 20;
  syncState.last_cam_crank_counter = 7; // Delta = 13

  detectSync();

  TEST_ASSERT_FALSE(syncState.synced);
}

// ============================================================================
// Test: Sync Detection Sequence
// ============================================================================
void test_sync_detection_full_sequence(void) {
  uint32_t time = 0;

  // Simulate crank teeth arriving
  for (int i = 0; i < 12; i++) {
    mock_us_timer_set_time(time);
    on_crank_tooth();
    time += 1000;
  }

  // First cam tooth (at crank counter 12)
  mock_us_timer_set_time(time);
  on_cam_tooth();

  // More crank teeth
  for (int i = 0; i < 12; i++) {
    mock_us_timer_set_time(time);
    on_crank_tooth();
    time += 1000;
  }

  // Second cam tooth (at crank counter 24, delta = 12)
  mock_us_timer_set_time(time);
  on_cam_tooth();

  // Now detect sync
  detectSync();

  TEST_ASSERT_TRUE(syncState.synced);
  TEST_ASSERT_EQUAL_UINT8(0, syncState.crank_index);
  TEST_ASSERT_FALSE(syncState.engine_phase);
}

// ============================================================================
// Test: Sync Re-Detection After Loss
// ============================================================================
void test_sync_can_be_reestablished_after_loss(void) {
  // First, establish sync
  syncState.cam_crank_counter = 12;
  syncState.last_cam_crank_counter = 0; // Delta = 12
  detectSync();
  TEST_ASSERT_TRUE(syncState.synced);

  // Lose sync with invalid delta
  syncState.cam_crank_counter = 20;
  syncState.last_cam_crank_counter = 15; // Delta = 5 (invalid)
  detectSync();
  TEST_ASSERT_FALSE(syncState.synced);

  // Reestablish sync
  syncState.cam_crank_counter = 31;
  syncState.last_cam_crank_counter = 21; // Delta = 10
  detectSync();
  TEST_ASSERT_TRUE(syncState.synced);
  TEST_ASSERT_EQUAL_UINT8(10, syncState.crank_index);
}

// ============================================================================
// Test: Edge Cases
// ============================================================================
void test_sync_with_counter_overflow_protection(void) {
  // Test with large counter values (near overflow)
  syncState.cam_crank_counter = 0xFFFFFFFFFFFFFFFF;           // Max uint64
  syncState.last_cam_crank_counter = 0xFFFFFFFFFFFFFFFF - 12; // Delta = 12

  uint8_t delta = syncState.get_cam_delta();
  TEST_ASSERT_EQUAL_UINT8(12, delta);

  detectSync();
  TEST_ASSERT_TRUE(syncState.synced);
}

// ============================================================================
// Main test runner
// ============================================================================
int main(void) {
  UNITY_BEGIN();

  // Sync detection with valid deltas
  RUN_TEST(test_detect_sync_with_delta_2_sets_correct_state);
  RUN_TEST(test_detect_sync_with_delta_10_sets_correct_state);
  RUN_TEST(test_detect_sync_with_delta_12_sets_correct_state);

  // Sync detection with invalid deltas
  RUN_TEST(test_detect_sync_with_invalid_delta_stays_not_synced);
  RUN_TEST(test_detect_sync_with_delta_0_stays_not_synced);
  RUN_TEST(test_detect_sync_with_delta_13_stays_not_synced);

  // Integration tests
  RUN_TEST(test_sync_detection_full_sequence);
  RUN_TEST(test_sync_can_be_reestablished_after_loss);

  // Edge cases
  RUN_TEST(test_sync_with_counter_overflow_protection);

  return UNITY_END();
}
