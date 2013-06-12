#ifndef _SYS_PROTO_H_
#define _SYS_PROTO_H_

#include <windef.h>

#include "sys_params.h"

#define __SYSCALL(name) extern int sys_##name(void *arg1, void *arg2,         \
                                              void *arg3, void *arg4,         \
                                              void *arg5, void *arg6);

#define __IMPLEMENTED(name, ...) extern int sys_##name(__VA_ARGS__, ...);

#include "syscalls.inc"

#undef __SYSCALL
#undef __IMPLEMENTED

#endif
