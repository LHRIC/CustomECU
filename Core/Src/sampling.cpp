#include "sampling.hpp"
#include "engine.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "ulog.h"
#include "us_timer.h"

// Tooth pattern is defined as 12 equally spaced
// crank teeth and 3 cam teeth, two opposing one
// 30deg offset.
#define NUM_CRANK_TEETH 12
#define DEG_BTWN_TEETH (uint8_t)(360 / NUM_CRANK_TEETH)

// Low-pass smoothing for tooth period calc. Range [0-1], smaller = more
// smoothing.
#define ALPHA 0.8

// Sync validation tolerance - tooth period can vary +/- this percentage
#define TOOTH_PERIOD_TOLERANCE 0.25 // 25% tolerance

struct SyncState syncState = {};

// Helper function to validate if current tooth timing is reasonable
static bool validate_tooth_timing(double dt) {
  if (syncState.tooth_period_us <= 0.0) {
    return true; // No reference period yet
  }

  // Check if the time between teeth is within tolerance
  double lower_bound =
      syncState.tooth_period_us * (1.0 - TOOTH_PERIOD_TOLERANCE);
  double upper_bound =
      syncState.tooth_period_us * (1.0 + TOOTH_PERIOD_TOLERANCE);

  return (dt >= lower_bound && dt <= upper_bound);
}

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
  // ULOG_DEBUG("Crank tooth detected");
  uint32_t current_time = get_micros();

  // Calculate time since last tooth
  bool timing_valid = true;

  // Only calculate period if we've seen at least one tooth before
  if (syncState.crank_counter > 0) {
    double dt = double(current_time - syncState.last_crank_time_us);

    // If synced, validate the tooth timing
    if (syncState.synced) {
      timing_valid = validate_tooth_timing(dt);

      if (!timing_valid) {
        // Lost sync due to missed tooth or timing anomaly
        syncState.synced = false;
        syncState.crank_index = 0;
        syncState.last_cam_crank_counter = 0;
        syncState.cam_crank_counter = 0;
        // Note: tooth_period_us is kept to help with re-sync
      }
    }

    // Update tooth period if timing is valid
    if (timing_valid) {
      if (syncState.tooth_period_us <= 0.0) {
        // First valid period measurement
        syncState.tooth_period_us = dt;
      } else {
        // Run a exponential moving average to smooth out jitter.
        syncState.tooth_period_us =
            ALPHA * dt + (1.0 - ALPHA) * syncState.tooth_period_us;
      }
    }
  }

  syncState.last_crank_time_us = current_time;
  syncState.crank_counter++;

  // Only update engine position if still synced
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

  uint8_t delta = syncState.get_cam_delta();

  // If we're synced, validate that the cam delta matches expectations
  if (syncState.synced && syncState.last_cam_crank_counter > 0) {
    // Valid deltas are 2, 10, or 12 teeth between cam pulses
    // Any other delta means we lost sync (missed teeth)
    if (delta != 2 && delta != 10 && delta != 12) {
      syncState.synced = false;
      syncState.crank_index = 0;
      syncState.last_cam_crank_counter = 0;
      syncState.cam_crank_counter = 0;
    }
  } else if (!syncState.synced && syncState.last_cam_crank_counter > 0) {
    // If we're not synced, try to acquire sync with this cam tooth
    detectSync();

    if (syncState.synced) {
      ULOG_INFO("Sync acquired! Crank index: %u, Phase: %u",
                syncState.crank_index, syncState.engine_phase);
    }
  }
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


void detectSync(void) {
  // Synchronization is determined when we see first three cam teeth.
  // Use num crank pulses between cam teeth detections to determine engine
  // cycle.
  //
  // diff btwn teeth == 330deg in crank: 0deg in cycle.
  // diff btwn teeth == 360deg in crank: 360deg into cycle.
  // diff btwn teeth == 30deg in crank: 390deg into cycle.
  uint8_t delta_teeth = syncState.get_cam_delta();
  switch (delta_teeth) {
  case 2:
    syncState.crank_index = 1;
    syncState.engine_phase = 1;
    syncState.synced = true;
    break;
  case 10:
    syncState.crank_index = 10;
    syncState.engine_phase = 0;
    syncState.synced = true;
    break;
  case 12:
    syncState.crank_index = 0;
    syncState.engine_phase = 0;
    syncState.synced = true;
    break;
  default:
    syncState.synced = false;
    break;
  }
}
