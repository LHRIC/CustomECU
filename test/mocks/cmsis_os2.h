#ifndef CMSIS_OS2_H
#define CMSIS_OS2_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

// CMSIS-RTOS2 status codes
typedef enum {
  osOK = 0,
  osError = -1,
  osErrorTimeout = -2,
  osErrorResource = -3,
  osErrorParameter = -4,
  osErrorNoMemory = -5,
  osErrorISR = -6,
} osStatus_t;

// CMSIS-RTOS2 thread priority
typedef enum {
  osPriorityNone = 0,
  osPriorityIdle = 1,
  osPriorityLow = 8,
  osPriorityBelowNormal = 16,
  osPriorityNormal = 24,
  osPriorityAboveNormal = 32,
  osPriorityHigh = 40,
  osPriorityRealtime = 48,
  osPriorityISR = 56,
  osPriorityError = -1,
} osPriority_t;

// CMSIS-RTOS2 thread ID
typedef void *osThreadId_t;

// CMSIS-RTOS2 thread attributes
typedef struct {
  const char *name;
  uint32_t attr_bits;
  void *cb_mem;
  uint32_t cb_size;
  void *stack_mem;
  uint32_t stack_size;
  osPriority_t priority;
  uint32_t reserved;
} osThreadAttr_t;

// CMSIS-RTOS2 function type for thread
typedef void (*osThreadFunc_t)(void *argument);

// Mock delay function
osStatus_t osDelay(uint32_t ticks);

// Mock thread functions
osThreadId_t osThreadNew(osThreadFunc_t func, void *argument,
                         const osThreadAttr_t *attr);
const char *osThreadGetName(osThreadId_t thread_id);
osThreadId_t osThreadGetId(void);
osStatus_t osThreadTerminate(osThreadId_t thread_id);

// Mock kernel functions
osStatus_t osKernelInitialize(void);
osStatus_t osKernelStart(void);
uint32_t osKernelGetTickCount(void);
uint32_t osKernelGetTickFreq(void);

#ifdef __cplusplus
}
#endif

#endif /* CMSIS_OS2_H */
