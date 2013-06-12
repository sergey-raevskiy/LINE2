#include <windef.h>
#include <stdlib.h>

#include "linux_errno.h"
#include "sys_proto.h"

typedef int (*SyscallHandler)(PVOID arg1, PVOID arg2,
                              PVOID arg3, PVOID arg4,
                              PVOID arg5, PVOID arg6);

static SyscallHandler SysEntry[] = {

#define __SYSCALL(name) sys_##name ,
#define __IMPLEMENTED(name, ...) __SYSCALL(name)
#include "syscalls.inc"
#undef __SYSCALL
#undef __IMPLEMENTED

};

static int DispatchSyscall(int SyscallId,
                           PVOID arg1, PVOID arg2,
                           PVOID arg3, PVOID arg4,
                           PVOID arg5, PVOID arg6)
{
    int NSys = _countof(SysEntry);

    if (SyscallId >= 0 || SyscallId < NSys)
    {
        return SysEntry[SyscallId](arg1, arg2, arg3, arg4, arg5, arg6);
    }
    else
    {
        printf("unexpected syscall: %d\n", SyscallId);
        exit(1);
    }
}

__declspec(naked) void HandleSyscall()
{
    __asm
    {
        /*
            Linux takes system call arguments in registers:

            syscall number  %eax     call-clobbered
            arg 1           %ebx     call-saved
            arg 2           %ecx     call-clobbered
            arg 3           %edx     call-clobbered
            arg 4           %esi     call-saved
            arg 5           %edi     call-saved
            arg 6           %ebp     call-saved
        */

        push ebp
        push edi
        push esi
        push edx
        push ecx
        push ebx

        push eax

        call DispatchSyscall

        add esp, 4
        pop ebx
        add esp, 8
        pop esi
        pop edi
        pop ebp

        ret
    }
}
