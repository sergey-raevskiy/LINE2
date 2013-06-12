#include "gs_support.h"
#include "sys_params.h"

#include <windows.h>
#include <stdio.h>

static DWORD CurrentGs()
{
    static DWORD GsTls = 0;

    // FIXME: Thread safety.

    if (!GsTls)
    {
        GsTls = TlsAlloc();
    }

    return GsTls;
}

void GSLoad(USHORT Value)
{
    DWORD TlsSlot;
    linux_user_desc_t *pDescriptor;

    TlsSlot = Value >> 3;
    pDescriptor = (linux_user_desc_t *) TlsGetValue(TlsSlot);

    TlsSetValue(CurrentGs(), pDescriptor);
}

static PVOID GSTranslateAddress(UINT Address)
{
    linux_user_desc_t *pDescriptor;

    pDescriptor = (linux_user_desc_t *) TlsGetValue(CurrentGs());

    return (PVOID) (pDescriptor->base_addr + Address);
}

void GSWriteDWord(UINT Address, DWORD Data)
{
    printf("write GS:[%p] = %d\n", Address, Data);
    *(PDWORD)GSTranslateAddress(Address) = Data;
}

DWORD GSReadDword(UINT Address)
{
    DWORD Result;

    Result = *(PDWORD)GSTranslateAddress(Address);
    printf("read GS:[%p] = %d\n", Address, Result);

    return Result;
}
