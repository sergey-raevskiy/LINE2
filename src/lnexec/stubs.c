#include <stdio.h>

#include "linux_errno.h"

#define __SYSCALL(name) int sys_##name(void *arg1, void *arg2,                \
                                       void *arg3, void *arg4,                \
                                       void *arg5, void *arg6)                \
{                                                                             \
    printf("sys_" #name ": stub called (%p, %p, %p, %p, %p, %p)\n",           \
           arg1, arg2, arg3, arg4, arg6, arg6);                               \
                                                                              \
    return -LINUX_ENOSYS;                                                     \
}

#include "syscalls.inc"

#undef __SYSCALL
