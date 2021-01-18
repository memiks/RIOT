/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   Implements various POSIX syscalls
 * @author  Gunar Schorcht <gunar@schorcht.net>
 */


/*
DESCRIPTION
Newlib was configured to allow the target platform to provide the locking
routines and static locks at link time.  As such, a dummy default
implementation of these routines and static locks is provided for
single-threaded application to link successfully out of the box on bare-metal
systems.

For multi-threaded applications the target platform is required to provide
an implementation for @strong{all} these routines and static locks.  If some
routines or static locks are missing, the link will fail with doubly defined
symbols.

PORTABILITY
These locking routines and static lock are newlib-specific.  Supporting OS
subroutines are required for linking multi-threaded applications.
*/

/* dummy lock routines and static locks for single-threaded apps */

//static mutex_t _lock;
//static rmutex_lock _rmut;

#ifndef __SINGLE_THREAD__

#include "lock.h"
#include <stdio.h>
#include "assert.h"
#include "irq.h"

mutex_t __lock___sinit_recursive_mutex;
mutex_t __lock___sfp_recursive_mutex;
mutex_t __lock___atexit_recursive_mutex;
mutex_t __lock___at_quick_exit_mutex;
mutex_t __lock___malloc_recursive_mutex;
mutex_t __lock___env_recursive_mutex;
mutex_t __lock___tz_mutex;
mutex_t __lock___dd_hash_mutex;
mutex_t __lock___arc4random_mutex;

#define UNUSED(x) (void)(x)


/*
static int locks_initialized = 0;

void init_newlib_locks(void ) {
    _lock_init(&__lock___sinit_recursive_mutex);
    _lock_init(&__lock___sfp_recursive_mutex);
    //_lock_init(&__lock___dd_hash_mutex);
    _lock_init(&__lock___atexit_recursive_mutex);

    _lock_init_recursive(&__lock___at_quick_exit_mutex);
    _lock_init_recursive(&__lock___malloc_recursive_mutex);
    _lock_init_recursive(&__lock___env_recursive_mutex);
    _lock_init_recursive(&__lock___tz_mutex);
    _lock_init_recursive(&__lock___arc4random_mutex);

    locks_initialized = 1;
}
*/

void
__retarget_lock_init (_LOCK_T *lock)
{
    unsigned int state = irq_disable();
    * lock = (_LOCK_T) &(struct __lock) {
        .mutex = NULL,
        .rmutex = NULL
    };

    mutex_init(&((*lock)->mutex));


    irq_restore(state);
}


void
__retarget_lock_init_recursive(_LOCK_T *lock)
{
    unsigned int state = irq_disable();
    *lock = &(struct __lock {
        .mutex = NULL,
        .rmutex = NULL
    });
    rmutex_init(&(*lock)->rmutex); 
    irq_restore(state);
}

/*
void
__retarget_lock_close(_LOCK_T lock)
{
    unsigned int state = irq_disable();
    mutex_cancel_init((lock)->mutex); 
    irq_restore(state);
}

void
__retarget_lock_close_recursive(_LOCK_T lock)
{
    unsigned int state = irq_disable();
    rmutex_unlock((lock)->rmutex);
    irq_restore(state);
}

void
__retarget_lock_acquire (_LOCK_T lock)
{
    unsigned int state = irq_disable();
    mutex_lock((lock)->mutex); 
    irq_restore(state);
}

void
__retarget_lock_acquire_recursive (_LOCK_T lock)
{
    unsigned int state = irq_disable();
    rmutex_lock((lock)->rmutex); 
    irq_restore(state);
}

int
__retarget_lock_try_acquire(_LOCK_T lock)
{
    assert(!irq_is_in());
    return mutex_trylock((lock)->mutex); 
}

int
__retarget_lock_try_acquire_recursive(_LOCK_T lock)
{
    assert(!irq_is_in());
    return rmutex_trylock((lock)->rmutex); 
}

void
__retarget_lock_release (_LOCK_T lock)
{
    unsigned int state = irq_disable();
    mutex_unlock((lock)->mutex); 
    irq_restore(state);
}

void
__retarget_lock_release_recursive (_LOCK_T lock)
{
    unsigned int state = irq_disable();
    rmutex_unlock((lock)->rmutex);
    irq_restore(state); 
}

*/

void
__retarget_lock_close(_LOCK_T lock)
{
//    puts("__retarget_lock_close");
    UNUSED(lock);
}

void
__retarget_lock_close_recursive(_LOCK_T lock)
{
//    puts("__retarget_lock_close_recursive");
    UNUSED(lock);
}

void
__retarget_lock_acquire (_LOCK_T lock)
{
//    puts("__retarget_lock_acquire");
    UNUSED(lock);
}

void
__retarget_lock_acquire_recursive (_LOCK_T lock)
{
//    puts("__retarget_lock_acquire_recursive");
    UNUSED(lock);
}

int
__retarget_lock_try_acquire(_LOCK_T lock)
{
//    puts("__retarget_lock_try_acquire");
    UNUSED(lock);
    return 1; 
}

int
__retarget_lock_try_acquire_recursive(_LOCK_T lock)
{
//    puts("__retarget_lock_try_acquire_recursive");
    UNUSED(lock);
    return 1; 
}

void
__retarget_lock_release (_LOCK_T lock)
{
//    puts("__retarget_lock_release");
    UNUSED(lock);
}

void
__retarget_lock_release_recursive (_LOCK_T lock)
{
//    puts("__retarget_lock_release_recursive");
    UNUSED(lock);
}


#endif /* !defined(__SINGLE_THREAD__) */
