/* SPDX-License-Identifier: MIT */

#ifndef COMPILER_H
#define COMPILER_H

#ifndef __ASSEMBLY__

#if __GNUC__ || __clang__


#define likely(x)           __builtin_expect (!!(x), 1)
#define unlikely(x)         __builtin_expect (!!(x), 0)

#define __is_constant(x)    __builtin_constant_p (x)


#define unreachable_barrier() asm volatile("")

#define unreachable()               \
    do {                            \
        unreachable_barrier();      \
        __builtin_unreachable();    \
    } while(0)


#define __fallthrough       __attribute__((__fallthrough__))

#define stringify(x)        #x


#define __unused            __attribute__((unused))
#define __used              __attribute__((used))

#define __packed            __attribute__((packed))

#define __noinline          __attribute__((noinline))
#define __always_inline     inline __attribute__((always_inline))

#define __noreturn          __attribute__((noreturn))
#define __malloc            __attribute__((malloc))
#define __weak              __attribute__((weak))

#define __constructor       __attribute__((constructor))
#define __destructor        __attribute__((destructor))


#define __pure              __attribute__ ((pure))
#define __hot               __attribute__ ((hot))
#define __cold              __attribute__ ((cold))
#define __const             __attribute__ ((const))


#define __visibility(x)     __attribute__ ((visibility (x)))
#define __aligned(x)        __attribute__ ((aligned(x)))
#define __section(x)        __attribute__ ((used, section(x)))
#define __allocsize(...)    __attribute__ ((alloc_size(__VA_ARGS__)))
#define __nonnull(...)      __attribute__ ((nonnull (__VA_ARGS__)))

#define __printf(fmt, va)   __attribute__((__format__(__printf__, fmt, va)))


#endif /* __GNUC__ || __clang__ */

#endif /* !__ASSEMBLY__ */

#endif /* !COMPILER_H */