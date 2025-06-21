#ifndef __TASKS_H
#define __TASKS_H

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
