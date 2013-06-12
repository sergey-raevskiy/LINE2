#include <windef.h>

#include "sys_proto.h"
#include "linux_errno.h"

#include <windows.h>

#define BRK_BLOCK_SIZE 4096

static void *CurrentBreak = NULL;
static void *Base = NULL;

// FIXME: return PVOID

int sys_brk(void *brk)
{
    PVOID Memory;

    if (brk == NULL)
    {
        Memory = VirtualAlloc(NULL,
                              10 * 1024 * 1024,
                              MEM_RESERVE,
                              PAGE_READWRITE);

        if (Memory)
        {
            Base = CurrentBreak = Memory;
        }
    }
    else
    {
        SIZE_T Size = (PBYTE)brk - (PBYTE)CurrentBreak;

        Memory = VirtualAlloc(Base,
                              Size,
                              MEM_COMMIT,
                              PAGE_READWRITE);

        if (Memory)
        {
            CurrentBreak = brk;
        }
    }

    return (int)CurrentBreak;
}
