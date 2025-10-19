#ifndef MOCK_HAL_H
#define MOCK_HAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Mock GPIO pin definitions
#define GPIO_PIN_0 0x0001
#define GPIO_PIN_1 0x0002
#define GPIO_PIN_2 0x0004

// HAL Status structures
typedef enum {
  HAL_OK = 0x00U,
  HAL_ERROR = 0x01U,
  HAL_BUSY = 0x02U,
  HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

// Mock GPIO typedef
typedef struct {
  uint32_t dummy;
} GPIO_TypeDef;

// Mock functions
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif // MOCK_HAL_H
