#ifndef __ENGINE_H
#define __ENGINE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// Function Prototypes

/**
 * @brief Function handling main critical engine task.
 * This task is responsible for using sampled sensor outputs to schedule
 * spark and fuel injection events.
 *
 * This task is critical for engine operation and should run at a high priority.
 * @param argument: Not used.
 * @retval None.
 */
void criticalEngineTask(void *argument);

#ifdef __cplusplus
}
#endif
#endif /* __ENGINE_H */
