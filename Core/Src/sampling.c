#include "sampling.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "us_timer.h"

volatile uint32_t last_crank_time_us = 0;
volatile uint32_t last_cam_time_us = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  switch (GPIO_Pin) {
  case CAM_SIGNAL_Pin:
    last_cam_time_us = get_micros();
    break;
  case CRANK_SIGNAL_Pin:
    last_crank_time_us = get_micros();
    break;
  default:
    break;
  }
}
