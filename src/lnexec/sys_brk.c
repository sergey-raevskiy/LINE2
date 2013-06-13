#include <windef.h>

#include "sys_proto.h"
#include "linux_errno.h"

#include <windows.h>

#define BRK_BLOCK_SIZE 4096

static void *CurrentBreak = NULL;

static PVOID PageAlign(PVOID Address)
{
    return (PVOID)((UINT)Address &~ 0xfff);
}

void SetInitialBreak(PVOID Break)
{
    PVOID Alloc = VirtualAlloc((PBYTE)PageAlign(Break) + (1 << 12),
                               10 * 1024 * 1024,
                               MEM_RESERVE,
                               PAGE_READWRITE);

    if (!Alloc)
    {
    //    abort();
    }

    CurrentBreak = Break;
}

// FIXME: return PVOID

int sys_brk(void *brk)
{
    if (PageAlign(CurrentBreak) < PageAlign(brk))
    {
        PVOID Alloc;
        SIZE_T Size;

        Size = (PBYTE)brk - (PBYTE)CurrentBreak;

        Alloc = VirtualAlloc(CurrentBreak,
                             Size,
                             MEM_RESERVE | MEM_COMMIT,
                             PAGE_READWRITE);

        //if (Alloc)
        {
            CurrentBreak = brk;
        }
    }
    else
    {
        if (brk)
        {
            CurrentBreak = brk;
        }
    }

    return (int)CurrentBreak;
}
