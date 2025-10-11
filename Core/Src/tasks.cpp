#include <cmsis_os2.h>
#include <stdint.h>

#include "sampling.hpp"
#include "tasks.h"
#include "ulog.h"
#include "us_timer.h"

const uint8_t cranking_rpm_threshold = (uint8_t)400;
const uint32_t cranking_rpm_threshold_us =
    (cranking_rpm_threshold / 60) * 1000000;

uint32_t last_time = 0;
uint32_t time = 0;

void detectSync(void) {
  // Synchronization is determined when we see first three cam teeth.
  // Use num crank pulses between cam teeth detections to determine engine
  // cycle.
  //
  // diff btwn teeth == 330deg in crank: 0deg in cycle.
  // diff btwn teeth == 360deg in crank: 360deg into cycle.
  // diff btwn teeth == 30deg in crank: 390deg into cycle.
  uint8_t delta_teeth = syncState.get_cam_delta();
  ULOG_DEBUG("Delta teeth: %d", delta_teeth);
  switch (delta_teeth) {
  case 1:
    syncState.crank_index = 1;
    syncState.engine_phase = 1;
    syncState.synced = true;
    break;
  case 11:
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

void criticalEngineTask(void *argument) {
  for (;;) {
    while (!syncState.synced) {
      detectSync();
    }

    time = get_micros();
    uint32_t dt = time - last_time;
    uint32_t dt_ms = dt / 1000;

    // Print every second.
    // if (dt >= 1000000) {
    //   last_time = time;
    //   ULOG_INFO("Detected Sync! Current Crank Angle: %f",
    //             syncState.current_engine_angle);
    //   ULOG_INFO("Fraction of tooth: %f", syncState.fraction_of_tooth);
    //   ULOG_INFO("Tooth period (us): %f", syncState.tooth_period_us);
    //   ULOG_INFO("Crank index: %d", syncState.crank_index);
    //   ULOG_INFO("Engine phase: %d", syncState.engine_phase);
    // }

    if (dt_ms >= 10) {
      last_time = time;
      float current_angle = get_current_engine_angle();
      float current_fraction = get_current_fraction_of_tooth();
      ULOG_DEBUG("Crank Angle: %f // Tooth Period: %f // Fraction: %f",
                 current_angle, syncState.tooth_period_us, current_fraction);
    }
  }
}
