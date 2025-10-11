#ifndef STM32F4XX_HAL_GPIO_H
#define STM32F4XX_HAL_GPIO_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// GPIO Init structure definition
typedef struct {
  uint32_t Pin;
  uint32_t Mode;
  uint32_t Pull;
  uint32_t Speed;
  uint32_t Alternate;
} GPIO_InitTypeDef;

// GPIO mode definitions
#define GPIO_MODE_INPUT 0x00000000U
#define GPIO_MODE_OUTPUT_PP 0x00000001U
#define GPIO_MODE_OUTPUT_OD 0x00000011U
#define GPIO_MODE_AF_PP 0x00000002U
#define GPIO_MODE_AF_OD 0x00000012U
#define GPIO_MODE_ANALOG 0x00000003U
#define GPIO_MODE_IT_RISING 0x10110000U
#define GPIO_MODE_IT_FALLING 0x10210000U
#define GPIO_MODE_IT_RISING_FALLING 0x10310000U
#define GPIO_MODE_EVT_RISING 0x10120000U
#define GPIO_MODE_EVT_FALLING 0x10220000U
#define GPIO_MODE_EVT_RISING_FALLING 0x10320000U

// GPIO pull definitions
#define GPIO_NOPULL 0x00000000U
#define GPIO_PULLUP 0x00000001U
#define GPIO_PULLDOWN 0x00000002U

// GPIO speed definitions
#define GPIO_SPEED_FREQ_LOW 0x00000000U
#define GPIO_SPEED_FREQ_MEDIUM 0x00000001U
#define GPIO_SPEED_FREQ_HIGH 0x00000002U
#define GPIO_SPEED_FREQ_VERY_HIGH 0x00000003U

// Mock function declarations
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                       GPIO_PinState PinState);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* STM32F4XX_HAL_GPIO_H */
