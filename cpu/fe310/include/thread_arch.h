/*
 * Copyright (C) 2021 Koen Zandberg <koen@bergzand.net>
 *               2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file
 * @brief       Implementation of the kernels thread interface
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
#ifndef THREAD_ARCH_H
#define THREAD_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_API_INLINED

#ifndef DOXYGEN /* Doxygen is in core/include/thread.h */

static inline void _ecall_dispatch(uint32_t num, void *ctx)
{
    /* function arguments are in a0 and a1 as per ABI */
    __asm__ volatile (
        "mv a0, %[num] \n"
        "mv a1, %[ctx] \n"
        "ECALL\n"
        : /* No outputs */
        : [num] "r" (num), [ctx] "r" (ctx)
        : "memory"
        );
}

static inline __attribute__((always_inline)) void thread_yield_higher(void)
{
    _ecall_dispatch(0, NULL);
}

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* THREAD_ARCH_H */
/** @} */
