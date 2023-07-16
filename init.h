/* SPDX-License-Identifier: MIT */

#pragma once

#include <stdint.h>
#include <compiler.h>

/**
 * Reperesents the different stages of the kernel initialization process.
 * This allows us to register hooks at a specific stage.
 */
typedef enum init_stage {
    INIT_STAGE_NONE = 0, /* No initialization. */
    INIT_STAGE_ARCH, /* Kernel early architecture-specific initializations. */
    INIT_STAGE_KPRINTF, /* Kernel printf is working. */
    INIT_STAGE_VM,      /* Kernel virtual memory is available. */
    INIT_STAGE_HEAP,    /* Kernel heap is initialized. */
    INIT_STAGE_PERCPU,  /* Kernel percpu initialization. */
} init_stage_t;

/**
 * Current kernel initialization stage.
 */
extern init_stage_t stage;

/**
 * Stage hooks are used to register callbacks at specific stages of
 * the kernel initialization process.
 */
typedef struct init_stage_hook {
    init_stage_t stage;
    uint32_t     priority;
    void (*hook)(const void *arg);
    const void *args;
    const char *name;
} init_stage_hook_t;

/**
 * All initialization code and initialized goes to the ".init" section.
 * Once we have initialized the kernel we can free this section.
 */
#define __init_func __section(".init.text")
#define __init_data __section(".init.data")
#define __init_hook __section(".init.hook")

/**
 * Registers a hook function at a specific initialization stage.
 *
 * @param _name Name of this hook.
 *
 * @param _stage Stage at which this hook would be registered.
 *
 * @param _priority Hook priority. Hooks with higher priority that are
 * registered in the same stage gets called earlier.
 *
 * @param _fn Pointer to the hook function.
 *
 * @param _args Pointer to the arguments passed to the hook function.
 */
#define REGISTER_INIT_HOOK(_name, _stage, _priority, _fn, _args)               \
    static __init_hook __aligned(8) init_stage_hook_t __init_hook_##_name = {  \
        .stage = INIT_STAGE_##_stage,                                          \
        .priority = _priority,                                                 \
        .hook = _fn,                                                           \
        .args = _args,                                                         \
        .name = stringify(_name),                                              \
    };

/**
 * Calls the initialization hooks upto the specified stage.
 *
 * @param upto_stage Stage upto which the initialization hooks are called.
 */
void kernel_init_upto(init_stage_t upto_stage);

/**
 * Initializes the kernel init stage hook system.
 */
void init_hooks_init(void);
