#ifndef TEST_PATCHER_H
#define TEST_PATCHER_H
#include <stdint.h>
typedef struct Patcher { int dummy; } Patcher;
static inline void Patcher_Construct(Patcher *p) {(void)p;}
static inline void Patcher_Install_Patch(Patcher *p, uint64_t address, uint8_t *bytes, unsigned long size) { (void)p; (void)address; (void)bytes; (void)size; }
static inline void Patcher_Destroy(Patcher *p) {(void)p;}
#endif
