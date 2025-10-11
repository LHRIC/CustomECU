#ifndef ULOG_H
#define ULOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

// Mock ULOG macros for testing - just use printf
#define ULOG_DEBUG(...)                                                        \
  printf("[DEBUG] " __VA_ARGS__);                                              \
  printf("\n")
#define ULOG_INFO(...)                                                         \
  printf("[INFO] " __VA_ARGS__);                                               \
  printf("\n")
#define ULOG_WARN(...)                                                         \
  printf("[WARN] " __VA_ARGS__);                                               \
  printf("\n")
#define ULOG_ERROR(...)                                                        \
  printf("[ERROR] " __VA_ARGS__);                                              \
  printf("\n")

// Mock function declarations (if any)
void ulog_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ULOG_H */
