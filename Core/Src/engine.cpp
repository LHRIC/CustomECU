#include <cmsis_os2.h>
#include <stdint.h>

#include "engine.h"
#include "sampling.hpp"
#include "ulog.h"
#include "us_timer.h"

uint32_t last_time = 0;
uint32_t time = 0;

void criticalEngineTask(void *argument) {
  (void)argument; // Unused parameter

  ULOG_INFO("Engine task started - waiting for sync...");

  for (;;) {
    // Wait for sync to be acquired by cam tooth interrupts
    if (!syncState.synced) {
      // Sleep briefly to avoid busy-waiting
      osDelay(1);
      continue;
    }

    // We're synced - perform engine control operations
    float current_angle = get_current_engine_angle();

    // TODO: Schedule fuel injection events
    // TODO: Schedule ignition events

    // ULOG_INFO("EngAngle: %.2f", current_angle);

    if (!syncState.synced) {
      ULOG_WARNING("Lost sync! Re-acquiring...");
      continue;
    }

    // Sleep briefly - actual timing is interrupt-driven
    osDelay(1);
  }
}
