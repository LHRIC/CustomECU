#ifndef __US_TIMER_H
#define __US_TIMER_H
#include <stdint.h>

/**
 * @brief Initializes the microsecond timer.
 * @retval None.
 */
void init_us_timer(void);

/**
 * @brief Get the current microsecond timer value.
 * @retval timer value.
 */
uint32_t get_micros(void);

#endif
