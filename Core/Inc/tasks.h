#ifndef __TASKS_H
#define __TASKS_H

#include "cmsis_os2.h"

// FreeRTOS function handles/definitions
osThreadId_t criticalEngineTaskHandle;
const osThreadAttr_t criticalEngineTaskAttributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

// Function Prototypes
/**
 * @brief Function handling main critical engine task.
 * This task is responsible for using sampled sensor outputs to schedule
 * spark and fuel injection events.
 *
 * This task is critical for engine operation and should run at a high priority.
 * @param argument: Not used
 * @retval None
 */
void criticalEngineTask(void *argument);

#endif
