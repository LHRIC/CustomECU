#include "mock_hal.h"
#include "mock_main.h"
#include "mock_us_timer.h"
#include "unity.h"

// Define main.h before including sampling
#define main_h
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

  // Reset mock timer
  mock_us_timer_set_time(0);
}

void tearDown(void) {
  // Cleanup after each test
}

// ============================================================================
// Test: Initial State
// ============================================================================
void test_initial_state_is_not_synced(void) {
  TEST_ASSERT_FALSE(syncState.synced);
  TEST_ASSERT_EQUAL_UINT8(0, syncState.crank_index);
  TEST_ASSERT_EQUAL_UINT64(0, syncState.crank_counter);
  TEST_ASSERT_EQUAL_DOUBLE(0.0, syncState.tooth_period_us);
}

// ============================================================================
// Test: Crank Tooth Detection
// ============================================================================
void test_on_crank_tooth_increments_counter(void) {
  mock_us_timer_set_time(1000);
  on_crank_tooth();

  TEST_ASSERT_EQUAL_UINT64(1, syncState.crank_counter);
  TEST_ASSERT_EQUAL_UINT32(1000, syncState.last_crank_time_us);
}

void test_on_crank_tooth_calculates_initial_period(void) {
  // First tooth at 1000us
  mock_us_timer_set_time(1000);
  on_crank_tooth();

  // Second tooth at 2500us (1500us period)
  mock_us_timer_set_time(2500);
  on_crank_tooth();

  TEST_ASSERT_EQUAL_DOUBLE(1500.0, syncState.tooth_period_us);
}

void test_on_crank_tooth_applies_exponential_moving_average(void) {
  // First tooth - establishes initial period
  mock_us_timer_set_time(1000);
  on_crank_tooth();

  mock_us_timer_set_time(2500); // dt = 1500us
  on_crank_tooth();
  TEST_ASSERT_EQUAL_DOUBLE(1500.0, syncState.tooth_period_us);

  // Third tooth with different period
  mock_us_timer_set_time(4200); // dt = 1700us
  on_crank_tooth();

  // Expected: 0.8 * 1700 + 0.2 * 1500 = 1360 + 300 = 1660
  TEST_ASSERT_DOUBLE_WITHIN(0.1, 1660.0, syncState.tooth_period_us);
}

void test_on_crank_tooth_wraps_crank_index_when_synced(void) {
  syncState.synced = true;
  syncState.crank_index = 11; // Last tooth (0-indexed)

  mock_us_timer_set_time(1000);
  on_crank_tooth();

  TEST_ASSERT_EQUAL_UINT8(0, syncState.crank_index);
}

void test_on_crank_tooth_toggles_engine_phase_at_tooth_11(void) {
  syncState.synced = true;
  syncState.crank_index = 11;
  syncState.engine_phase = false;

  mock_us_timer_set_time(1000);
  on_crank_tooth();

  TEST_ASSERT_TRUE(syncState.engine_phase);
  TEST_ASSERT_EQUAL_UINT8(0, syncState.crank_index);
}

void test_on_crank_tooth_does_not_change_index_when_not_synced(void) {
  syncState.synced = false;
  syncState.crank_index = 5;

  mock_us_timer_set_time(1000);
  on_crank_tooth();

  // Index should still be 5 since we're not synced
  TEST_ASSERT_EQUAL_UINT8(5, syncState.crank_index);
}

// ============================================================================
// Test: Cam Tooth Detection
// ============================================================================
void test_on_cam_tooth_updates_counters(void) {
  syncState.crank_counter = 10;
  syncState.cam_crank_counter = 5;

  mock_us_timer_set_time(5000);
  on_cam_tooth();

  TEST_ASSERT_EQUAL_UINT64(5, syncState.last_cam_crank_counter);
  TEST_ASSERT_EQUAL_UINT64(10, syncState.cam_crank_counter);
  TEST_ASSERT_EQUAL_UINT32(5000, syncState.last_cam_time_us);
}

void test_get_cam_delta_calculates_difference(void) {
  syncState.cam_crank_counter = 15;
  syncState.last_cam_crank_counter = 3;

  uint8_t delta = syncState.get_cam_delta();

  TEST_ASSERT_EQUAL_UINT8(12, delta);
}

// ============================================================================
// Test: Fraction of Tooth Calculation
// ============================================================================
void test_get_current_fraction_when_no_period_set(void) {
  syncState.tooth_period_us = 0.0;

  float fraction = get_current_fraction_of_tooth();

  TEST_ASSERT_EQUAL_FLOAT(0.0f, fraction);
}

void test_get_current_fraction_at_quarter_period(void) {
  syncState.tooth_period_us = 1000.0;
  syncState.last_crank_time_us = 1000;

  mock_us_timer_set_time(1250); // 250us after last tooth = 0.25 fraction

  float fraction = get_current_fraction_of_tooth();

  TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.25f, fraction);
}

void test_get_current_fraction_at_half_period(void) {
  syncState.tooth_period_us = 1000.0;
  syncState.last_crank_time_us = 2000;

  mock_us_timer_set_time(2500); // 500us after last tooth = 0.5 fraction

  float fraction = get_current_fraction_of_tooth();

  TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.5f, fraction);
}

void test_get_current_fraction_clamps_to_one(void) {
  syncState.tooth_period_us = 1000.0;
  syncState.last_crank_time_us = 1000;

  mock_us_timer_set_time(3000); // 2000us after = 2.0, should clamp to 1.0

  float fraction = get_current_fraction_of_tooth();

  TEST_ASSERT_EQUAL_FLOAT(1.0f, fraction);
}

void test_get_current_fraction_clamps_negative_to_zero(void) {
  syncState.tooth_period_us = 1000.0;
  syncState.last_crank_time_us = 5000;

  mock_us_timer_set_time(
      4000); // Negative dt (shouldn't happen but test robustness)

  float fraction = get_current_fraction_of_tooth();

  TEST_ASSERT_EQUAL_FLOAT(0.0f, fraction);
}

// ============================================================================
// Test: Engine Angle Calculation
// ============================================================================
void test_get_current_engine_angle_returns_zero_when_not_synced(void) {
  syncState.synced = false;
  syncState.crank_index = 5;
  syncState.engine_phase = 1;

  float angle = get_current_engine_angle();

  TEST_ASSERT_EQUAL_FLOAT(0.0f, angle);
}

void test_get_current_engine_angle_at_tooth_zero_phase_zero(void) {
  syncState.synced = true;
  syncState.crank_index = 0;
  syncState.engine_phase = 0;
  syncState.tooth_period_us = 1000.0;
  syncState.last_crank_time_us = 1000;

  mock_us_timer_set_time(1000); // Right at tooth, fraction = 0

  float angle = get_current_engine_angle();

  // Phase 0 * 360 + Index 0 * 30 + Fraction 0 * 30 = 0
  TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, angle);
}

void test_get_current_engine_angle_at_tooth_one_phase_zero(void) {
  syncState.synced = true;
  syncState.crank_index = 1;
  syncState.engine_phase = 0;
  syncState.tooth_period_us = 1000.0;
  syncState.last_crank_time_us = 1000;

  mock_us_timer_set_time(1000); // Right at tooth, fraction = 0

  float angle = get_current_engine_angle();

  // Phase 0 * 360 + Index 1 * 30 + Fraction 0 * 30 = 30
  TEST_ASSERT_FLOAT_WITHIN(0.1f, 30.0f, angle);
}

void test_get_current_engine_angle_at_tooth_six_phase_zero(void) {
  syncState.synced = true;
  syncState.crank_index = 6;
  syncState.engine_phase = 0;
  syncState.tooth_period_us = 1000.0;
  syncState.last_crank_time_us = 1000;

  mock_us_timer_set_time(1000);

  float angle = get_current_engine_angle();

  // Phase 0 * 360 + Index 6 * 30 + Fraction 0 * 30 = 180
  TEST_ASSERT_FLOAT_WITHIN(0.1f, 180.0f, angle);
}

void test_get_current_engine_angle_at_tooth_zero_phase_one(void) {
  syncState.synced = true;
  syncState.crank_index = 0;
  syncState.engine_phase = 1;
  syncState.tooth_period_us = 1000.0;
  syncState.last_crank_time_us = 1000;

  mock_us_timer_set_time(1000);

  float angle = get_current_engine_angle();

  // Phase 1 * 360 + Index 0 * 30 + Fraction 0 * 30 = 360
  TEST_ASSERT_FLOAT_WITHIN(0.1f, 360.0f, angle);
}

void test_get_current_engine_angle_with_fraction(void) {
  syncState.synced = true;
  syncState.crank_index = 2;
  syncState.engine_phase = 0;
  syncState.tooth_period_us = 1000.0;
  syncState.last_crank_time_us = 1000;

  mock_us_timer_set_time(1500); // Fraction = 0.5

  float angle = get_current_engine_angle();

  // Phase 0 * 360 + Index 2 * 30 + Fraction 0.5 * 30 = 60 + 15 = 75
  TEST_ASSERT_FLOAT_WITHIN(0.1f, 75.0f, angle);
}

void test_get_current_engine_angle_near_end_of_cycle(void) {
  syncState.synced = true;
  syncState.crank_index = 11;
  syncState.engine_phase = 1;
  syncState.tooth_period_us = 1000.0;
  syncState.last_crank_time_us = 1000;

  mock_us_timer_set_time(1750); // Fraction = 0.75

  float angle = get_current_engine_angle();

  // Phase 1 * 360 + Index 11 * 30 + Fraction 0.75 * 30 = 360 + 330 + 22.5 =
  // 712.5
  TEST_ASSERT_FLOAT_WITHIN(0.1f, 712.5f, angle);
}

// ============================================================================
// Test: Complete Rotation Scenario
// ============================================================================
void test_complete_rotation_scenario(void) {
  uint32_t time = 0;
  uint32_t tooth_period = 1000; // 1000us between teeth

  syncState.synced = true;
  syncState.crank_index = 0;
  syncState.engine_phase = 0;

  // Simulate 12 crank teeth (one full rotation)
  for (int i = 0; i < 12; i++) {
    mock_us_timer_set_time(time);
    on_crank_tooth();
    time += tooth_period;
  }

  // After 12 teeth, we should be back at index 0 with phase toggled
  TEST_ASSERT_EQUAL_UINT8(0, syncState.crank_index);
  TEST_ASSERT_TRUE(syncState.engine_phase);
  TEST_ASSERT_EQUAL_UINT64(12, syncState.crank_counter);
}

// ============================================================================
// Main test runner
// ============================================================================
int main(void) {
  UNITY_BEGIN();

  // Initial state tests
  RUN_TEST(test_initial_state_is_not_synced);

  // Crank tooth detection tests
  RUN_TEST(test_on_crank_tooth_increments_counter);
  RUN_TEST(test_on_crank_tooth_calculates_initial_period);
  RUN_TEST(test_on_crank_tooth_applies_exponential_moving_average);
  RUN_TEST(test_on_crank_tooth_wraps_crank_index_when_synced);
  RUN_TEST(test_on_crank_tooth_toggles_engine_phase_at_tooth_11);
  RUN_TEST(test_on_crank_tooth_does_not_change_index_when_not_synced);

  // Cam tooth detection tests
  RUN_TEST(test_on_cam_tooth_updates_counters);
  RUN_TEST(test_get_cam_delta_calculates_difference);

  // Fraction of tooth tests
  RUN_TEST(test_get_current_fraction_when_no_period_set);
  RUN_TEST(test_get_current_fraction_at_quarter_period);
  RUN_TEST(test_get_current_fraction_at_half_period);
  RUN_TEST(test_get_current_fraction_clamps_to_one);
  RUN_TEST(test_get_current_fraction_clamps_negative_to_zero);

  // Engine angle calculation tests
  RUN_TEST(test_get_current_engine_angle_returns_zero_when_not_synced);
  RUN_TEST(test_get_current_engine_angle_at_tooth_zero_phase_zero);
  RUN_TEST(test_get_current_engine_angle_at_tooth_one_phase_zero);
  RUN_TEST(test_get_current_engine_angle_at_tooth_six_phase_zero);
  RUN_TEST(test_get_current_engine_angle_at_tooth_zero_phase_one);
  RUN_TEST(test_get_current_engine_angle_with_fraction);
  RUN_TEST(test_get_current_engine_angle_near_end_of_cycle);

  // Integration tests
  RUN_TEST(test_complete_rotation_scenario);

  return UNITY_END();
}
