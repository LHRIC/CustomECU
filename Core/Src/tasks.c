#include <cmsis_os2.h>
#include <stdbool.h>
#include <stdint.h>

#include "sampling.h"
#include "tasks.h"
#include "us_timer.h"
#include "ulog.h"

const uint8_t cranking_rpm_threshold = (uint8_t) 400;
const uint32_t cranking_rpm_threshold_us = (cranking_rpm_threshold / 60) * 1000000;

void criticalEngineTask(void *argument) {
  for (;;) {
    const uint32_t cur_micros = get_micros();
    bool is_engine_turning =
        cur_micros - last_crank_time_us > cranking_rpm_threshold_us;

    ULOG_DEBUG("CRANK TIME: %d", last_crank_time_us);

    if (!is_engine_turning) {
      // TODO: Run wasted spark/semi-sequential.
    }

    // TODO: Run nominal engine injection and spark.
  }
}
