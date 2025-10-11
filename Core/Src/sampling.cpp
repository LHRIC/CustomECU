#include "sampling.hpp"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "us_timer.h"

// Tooth pattern is defined as 12 equally spaced
// crank teeth and 3 cam teeth, two opposing one
// 30deg offset.
#define NUM_CRANK_TEETH 12
#define DEG_BTWN_TEETH (uint8_t)(360 / NUM_CRANK_TEETH)

// Low-pass smoothing for tooth period calc. Range [0-1], smaller = more
// smoothing.
#define ALPHA 0.8

struct SyncState syncState = {};

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  switch (GPIO_Pin) {
  case CAM_SIGNAL_Pin:
    on_cam_tooth();
    break;
  case CRANK_SIGNAL_Pin:
    on_crank_tooth();
    break;
  default:
    break;
  }
}

void on_crank_tooth() {
  uint32_t current_time = get_micros();

  // Only calculate period if we've seen at least one tooth before
  if (syncState.crank_counter > 0) {
    double dt = double(current_time - syncState.last_crank_time_us);

    if (syncState.tooth_period_us <= 0.0) {
      // First valid period measurement
      syncState.tooth_period_us = dt;
    } else {
      // Run a exponential moving average to smooth out jitter.
      syncState.tooth_period_us =
          ALPHA * dt + (1.0 - ALPHA) * syncState.tooth_period_us;
    }
  }

  syncState.last_crank_time_us = current_time;
  syncState.crank_counter++;

  if (syncState.synced) {
    if (syncState.crank_index == NUM_CRANK_TEETH - 1) {
      syncState.engine_phase = !syncState.engine_phase;
    }
    syncState.crank_index = (syncState.crank_index + 1) % NUM_CRANK_TEETH;
  }
}

void on_cam_tooth() {
  syncState.last_cam_time_us = get_micros();
  syncState.last_cam_crank_counter = syncState.cam_crank_counter;
  syncState.cam_crank_counter = syncState.crank_counter;
}

float get_current_fraction_of_tooth() {
  if (syncState.tooth_period_us <= 0.0) {
    return 0.0f;
  }

  uint32_t current_time = get_micros();

  // Handle case where current_time is less than last_crank_time_us
  // (shouldn't happen in normal operation, but guard against it)
  if (current_time < syncState.last_crank_time_us) {
    return 0.0f;
  }

  float dt = (float)(current_time - syncState.last_crank_time_us);
  float fraction = dt / (float)syncState.tooth_period_us;

  // Clamp between 0 and 1
  if (fraction > 1.0f) {
    fraction = 1.0f;
  }

  return fraction;
}

float get_current_engine_angle() {
  if (!syncState.synced) {
    return 0.0f;
  }

  float current_fraction = get_current_fraction_of_tooth();
  float angle = (syncState.engine_phase * 360.0f) +
                (syncState.crank_index * DEG_BTWN_TEETH) +
                (current_fraction * DEG_BTWN_TEETH);

  return angle;
}
