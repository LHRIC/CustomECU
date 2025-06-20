#include "main.h"
#include "stm32f4xx_hal_gpio.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  switch (GPIO_Pin) {
  case CAM_SIGNAL_Pin:
    break;
  case CRANK_SIGNAL_Pin:
    break;
  default:
    break;
  }
}
