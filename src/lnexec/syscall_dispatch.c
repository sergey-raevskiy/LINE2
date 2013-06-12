#include <windef.h>
#include <stdlib.h>

#include "linux_errno.h"
#include "sys_proto.h"

typedef int (*SyscallHandler)(PVOID arg1, PVOID arg2,
                              PVOID arg3, PVOID arg4,
                              PVOID arg5, PVOID arg6);

static SyscallHandler SysEntry[] = {

#define __SYSCALL(name) sys_##name ,
#include "syscalls.inc"
#undef __SYSCALL

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
        return -LINUX_ENOSYS;
    }
}

__declspec(naked) void HandleSyscall()
{
    __asm
    {
        push ebp
        push edi
        push esi
        push edx
        push ecx
        push ebx

        push eax

        call DispatchSyscall

        add esp, 7*4
        ret
    }
}
