#include "stm32f4xx_hal_gpio.h"

// Mock implementations
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init) {
  (void)GPIOx;
  (void)GPIO_Init;
}

void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin) {
  (void)GPIOx;
  (void)GPIO_Pin;
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
  (void)GPIOx;
  (void)GPIO_Pin;
  return GPIO_PIN_RESET;
}

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                       GPIO_PinState PinState) {
  (void)GPIOx;
  (void)GPIO_Pin;
  (void)PinState;
}

void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
  (void)GPIOx;
  (void)GPIO_Pin;
}

void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin) {
  HAL_GPIO_EXTI_Callback(GPIO_Pin);
}

// Weak implementation - can be overridden in tests
__attribute__((weak)) void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  (void)GPIO_Pin;
}
