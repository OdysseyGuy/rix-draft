#include <init.h>
#include <stddef.h>
#include <stdint.h>

/* kernel.lds.S */
extern init_stage_hook_t __init_hooks_start[];
extern init_stage_hook_t __init_hooks_end[];

/* current stage hook */
static __init_data init_stage_hook_t *init_hook_cur = __init_hooks_start;

init_stage_t stage = INIT_STAGE_NONE;

static int __init_func hook_cmp(const void *hook1, const void *hook2)
{
    const init_stage_hook_t *a = hook1;
    const init_stage_hook_t *b = hook2;

    if (a->stage == b->stage) {
        if (a->priority == b->priority) {
            return 0;
        }
        return a->priority < b->priority ? -1 : 1;
    }
    return a->stage < b->stage ? -1 : 1;
}

extern void qsort(void *arr, size_t n, size_t es, int (*cmp)(const void *, const void *));

/* TODO: Sort this at build time maybe. */
void __init_func init_hooks_init(void)
{
    size_t num_hooks = __init_hooks_end - __init_hooks_start;

    if (((uintptr_t)__init_hooks_end - (uintptr_t)__init_hooks_start)
        % sizeof(init_stage_hook_t)) {
    }

    /* sort the hooks in level of their stages */
    qsort(__init_hooks_start, num_hooks, sizeof(init_stage_hook_t), hook_cmp);
}

void __init_func kernel_init_upto(init_stage_t upto_stage)
{
    init_stage_hook_t *cur_hook = init_hook_cur;

    while (cur_hook < __init_hooks_end && cur_hook->stage <= upto_stage) {
        stage = cur_hook->stage - 1;
        cur_hook->hook(cur_hook->args);
        init_hook_cur = ++cur_hook;
    }

    stage = upto_stage;
}


#if 0

void __init_func init_test1()
{
    asm volatile("int3");
}

void __init_func init_test2()
{
    asm volatile("int3");
}

void __init_func init_test3()
{
    asm volatile("int3");
}

REGISTER_INIT_HOOK(test1, KPRINTF, 1, &init_test1, NULL);
REGISTER_INIT_HOOK(test2, KPRINTF, 2, &init_test2, NULL);
REGISTER_INIT_HOOK(test3, KPRINTF, 3, &init_test3, NULL);

#endif
