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

/**
 * @brief Handles synchronization detection with cam and crank signals.
 * Specifically for the Honda CBR600CC engine with 12 equally-spaced
 * crank teeth and 3 cam teeth, with one offset 30deg.
 *
 * Updates synced bool in SyncState struct.
 *
 * @param None.
 * @retval None.
 */
void detectSync(void);

#ifdef __cplusplus
}
#endif
#endif /* __ENGINE_H */
