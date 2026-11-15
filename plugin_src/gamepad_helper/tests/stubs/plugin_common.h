#ifndef TEST_PLUGIN_COMMON_H
#define TEST_PLUGIN_COMMON_H
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#define attr_public
#define attr_module_hidden
#define GOLDHEN_PATH "/data/GoldHEN"
typedef uint32_t u32;
typedef int32_t s32;
typedef int64_t s64;
#define final_printf(...) ((void)printf(__VA_ARGS__))
#define debug_printf(...) ((void)printf(__VA_ARGS__))
#define HOOK_INIT(name)
#define HOOK32(name) ((void)0)
#define UNHOOK(name) ((void)0)
#define HOOK_CONTINUE(name, type, ...) 0
struct proc_info { char titleid[32]; };
static inline int sys_sdk_proc_info(struct proc_info *i) { (void)i; return 0; }
static inline void print_proc_info(void) {}
static inline void boot_ver(void) {}
static inline const char *sceKernelGetFsSandboxRandomWord(void) { return "sandbox"; }
static inline int sys_dynlib_load_prx(const char* m, int* h) { (void)m; (void)h; return 0; }
#endif
