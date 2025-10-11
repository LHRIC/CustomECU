#ifndef __SAMPLING_H
#define __SAMPLING_H
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief  Callback for when a cam tooth is detected.
 * @param  None
 * @retval None
 */
void on_cam_tooth();

/**
 * @brief  Callback for when a crank tooth is detected.
 * @param  None
 * @retval None
 */
void on_crank_tooth();

/**
 * @brief  Get the current fraction of tooth passed since last crank tooth.
 * @param  None
 * @retval Fraction of tooth passed (0.0 to 1.0)
 */
float get_current_fraction_of_tooth();

/**
 * @brief  Get the current engine angle in degrees.
 * @param  None
 * @retval Current engine angle in degrees (0.0 to 720.0)
 */
float get_current_engine_angle();

// General Engine Synchronization State.
struct SyncState {
  // Whether we have locked synchronization or not.
  bool synced;

  // Crank index.
  volatile uint8_t crank_index = 0;

  // Monotonic crank counter.
  volatile uint64_t crank_counter = 0;

  // Monotonic cam crank counter.
  volatile uint64_t cam_crank_counter = 0;

  // Monotonic cam crank counter for last cam seen.
  volatile uint64_t last_cam_crank_counter = 0;

  // Last time in micros when we saw a crank tooth.
  volatile uint32_t last_crank_time_us = 0;

  // Last time in micros when we saw a cam tooth.
  volatile uint32_t last_cam_time_us = 0;

  // Instantaneous period between crank teeth (in microseconds).
  volatile double tooth_period_us = 0.0;

  // Engine phase (0 = 0-360, 1 = 360-720)
  bool engine_phase;

  // Gets the delta in crank teeth between the last two cam teeth.
  inline uint8_t get_cam_delta() {
    return cam_crank_counter - last_cam_crank_counter;
  };
};

extern struct SyncState syncState;

#endif
