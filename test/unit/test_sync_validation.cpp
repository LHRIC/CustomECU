#include "mock_main.h"
#include "mock_us_timer.h"
#include "unity.h"

// Include sampling header
#include "sampling.hpp"

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

  mock_us_timer_set_time(0);
}

void tearDown(void) {
  // Cleanup
}

// ============================================================================
// Test: Sync Loss on Missed Crank Tooth
// ============================================================================
void test_sync_lost_when_tooth_period_too_long(void) {
  // Establish sync
  mock_us_timer_set_time(0);
  on_crank_tooth();

  mock_us_timer_set_time(1000);
  on_crank_tooth();

  // Establish period of 1000us
  mock_us_timer_set_time(2000);
  on_crank_tooth();

  // Establish sync
  syncState.synced = true;
  syncState.crank_index = 5;

  // Next tooth comes way too late (2x expected) - missed a tooth
  mock_us_timer_set_time(4100); // 2100us instead of 1000us
  on_crank_tooth();

  // Should have lost sync
  TEST_ASSERT_FALSE(syncState.synced);
  TEST_ASSERT_EQUAL_UINT8(0, syncState.crank_index);
}

void test_sync_lost_when_tooth_period_too_short(void) {
  // Establish sync
  mock_us_timer_set_time(0);
  on_crank_tooth();

  mock_us_timer_set_time(1000);
  on_crank_tooth();

  mock_us_timer_set_time(2000);
  on_crank_tooth();

  syncState.synced = true;
  syncState.crank_index = 5;

  // Next tooth comes way too early (half expected)
  mock_us_timer_set_time(2400); // 400us instead of 1000us
  on_crank_tooth();

  // Should have lost sync
  TEST_ASSERT_FALSE(syncState.synced);
}

void test_sync_maintained_with_small_timing_variations(void) {
  // Establish period
  mock_us_timer_set_time(0);
  on_crank_tooth();

  mock_us_timer_set_time(1000);
  on_crank_tooth();

  mock_us_timer_set_time(2000);
  on_crank_tooth();

  syncState.synced = true;
  syncState.crank_index = 5;

  // Small variation within tolerance (10% faster)
  mock_us_timer_set_time(2900); // 900us (within 25% tolerance)
  on_crank_tooth();

  TEST_ASSERT_TRUE(syncState.synced);
  TEST_ASSERT_EQUAL_UINT8(6, syncState.crank_index);
}

// ============================================================================
// Test: Sync Loss on Invalid Cam Delta
// ============================================================================
void test_sync_lost_on_invalid_cam_delta(void) {
  // Set up synced state with valid tooth period
  mock_us_timer_set_time(1000);
  on_crank_tooth();
  mock_us_timer_set_time(2000);
  on_crank_tooth();

  syncState.synced = true;
  syncState.crank_index = 3;

  // First cam tooth
  syncState.cam_crank_counter = 10;
  syncState.last_cam_crank_counter = 0;
  on_cam_tooth();

  // Continue with crank teeth
  uint32_t time = 2000;
  for (int i = 0; i < 5; i++) {
    time += 1000;
    mock_us_timer_set_time(time);
    on_crank_tooth();
  }

  // Second cam tooth with invalid delta (5 teeth - invalid!)
  syncState.cam_crank_counter = syncState.crank_counter;
  on_cam_tooth();

  // Should have lost sync
  TEST_ASSERT_FALSE(syncState.synced);
  TEST_ASSERT_EQUAL_UINT8(0, syncState.crank_index);
}

void test_sync_maintained_on_valid_cam_delta(void) {
  // Set up synced state
  mock_us_timer_set_time(1000);
  on_crank_tooth();
  mock_us_timer_set_time(2000);
  on_crank_tooth();

  syncState.synced = true;
  syncState.crank_index = 0;

  // Simulate first cam tooth at current position
  syncState.last_cam_crank_counter = 0;
  syncState.cam_crank_counter =
      syncState.crank_counter; // Record current position

  // Continue with exactly 12 crank teeth
  uint32_t time = 2000;
  for (int i = 0; i < 12; i++) {
    time += 1000;
    mock_us_timer_set_time(time);
    on_crank_tooth();
  }

  // Second cam tooth with valid delta (12 teeth)
  on_cam_tooth();

  // Should maintain sync (delta should be 12)
  TEST_ASSERT_TRUE(syncState.synced);
}

// ============================================================================
// Test: Sync Re-establishment After Loss
// ============================================================================
void test_can_regain_sync_after_loss(void) {
  // Establish initial sync
  mock_us_timer_set_time(0);
  on_crank_tooth();
  mock_us_timer_set_time(1000);
  on_crank_tooth();
  mock_us_timer_set_time(2000);
  on_crank_tooth();

  syncState.synced = true;
  syncState.crank_index = 5;

  // Lose sync due to timing
  mock_us_timer_set_time(4500); // Way too long
  on_crank_tooth();

  TEST_ASSERT_FALSE(syncState.synced);

  // Now continue with regular teeth to re-establish pattern
  uint32_t time = 5500;
  for (int i = 0; i < 15; i++) {
    mock_us_timer_set_time(time);
    on_crank_tooth();
    time += 1000;
  }

  // Period should be re-established
  TEST_ASSERT_DOUBLE_WITHIN(100.0, 1000.0, syncState.tooth_period_us);

  // Manually re-sync (this would be done by detectSync in real system)
  syncState.synced = true;

  // Verify sync is maintained with good timing
  mock_us_timer_set_time(time);
  on_crank_tooth();

  TEST_ASSERT_TRUE(syncState.synced);
}

// ============================================================================
// Main test runner
// ============================================================================
int main(void) {
  UNITY_BEGIN();

  // Sync loss tests
  RUN_TEST(test_sync_lost_when_tooth_period_too_long);
  RUN_TEST(test_sync_lost_when_tooth_period_too_short);
  RUN_TEST(test_sync_maintained_with_small_timing_variations);

  // Cam delta validation tests
  RUN_TEST(test_sync_lost_on_invalid_cam_delta);
  RUN_TEST(test_sync_maintained_on_valid_cam_delta);

  // Recovery tests
  RUN_TEST(test_can_regain_sync_after_loss);

  return UNITY_END();
}
