#include "cmsis_os2.h"

// Mock implementations
osStatus_t osDelay(uint32_t ticks) {
  (void)ticks;
  return osOK;
}

osThreadId_t osThreadNew(osThreadFunc_t func, void *argument,
                         const osThreadAttr_t *attr) {
  (void)func;
  (void)argument;
  (void)attr;
  return (osThreadId_t)1; // Return dummy thread ID
}

const char *osThreadGetName(osThreadId_t thread_id) {
  (void)thread_id;
  return "MockThread";
}

osThreadId_t osThreadGetId(void) { return (osThreadId_t)1; }

osStatus_t osThreadTerminate(osThreadId_t thread_id) {
  (void)thread_id;
  return osOK;
}

osStatus_t osKernelInitialize(void) { return osOK; }

osStatus_t osKernelStart(void) { return osOK; }

uint32_t osKernelGetTickCount(void) { return 0; }

uint32_t osKernelGetTickFreq(void) {
  return 1000; // 1kHz
}
