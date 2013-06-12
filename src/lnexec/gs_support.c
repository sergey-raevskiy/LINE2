#include "gs_support.h"
#include "sys_params.h"

#include <windows.h>

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
