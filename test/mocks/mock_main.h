#ifndef MOCK_MAIN_H
#define MOCK_MAIN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Mock pin definitions
#define CAM_SIGNAL_Pin GPIO_PIN_0
#define CRANK_SIGNAL_Pin GPIO_PIN_1

// Mock GPIO definitions
#define GPIO_PIN_0 0x0001
#define GPIO_PIN_1 0x0002

// void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif // MOCK_MAIN_H
