#include "mock_us_timer.h"

static uint32_t mock_time_us = 0;

void mock_us_timer_set_time(uint32_t time_us) { mock_time_us = time_us; }

uint32_t mock_us_timer_get_time(void) { return mock_time_us; }

uint32_t get_micros(void) { return mock_time_us; }

void init_us_timer(void) {
  // Mock implementation - does nothing
  mock_time_us = 0;
}
