#include <cmsis_os2.h>
#include <stdint.h>

#include "sampling.hpp"
#include "tasks.h"
#include "ulog.h"
#include "us_timer.h"

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

    // TODO: Schedule fuel/ignition events based on current angle
    osDelay(1);
  }
}
