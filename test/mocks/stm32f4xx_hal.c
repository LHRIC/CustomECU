#include "stm32f4xx_hal.h"

// Mock implementations
uint32_t HAL_GetTick(void) { return 0; }

void HAL_Delay(uint32_t Delay) { (void)Delay; }

void Error_Handler(void) {
  // Do nothing in tests
}
