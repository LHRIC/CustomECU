#ifndef MOCK_US_TIMER_H
#define MOCK_US_TIMER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Mock function to control time in tests
void mock_us_timer_set_time(uint32_t time_us);
uint32_t mock_us_timer_get_time(void);

// Actual functions that will be called by code under test
uint32_t get_micros(void);
void init_us_timer(void);

#ifdef __cplusplus
}
#endif

#endif // MOCK_US_TIMER_H
