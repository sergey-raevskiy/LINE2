#include <windef.h>

#include "sys_proto.h"
#include "linux_errno.h"

#include <windows.h>
#include <stdio.h>

int sys_set_thread_area(linux_user_desc_t *udesc)
{
    linux_user_desc_t *pSegmentDescriptor;

    if (udesc->entry_number == -1)
    {
        udesc->entry_number = TlsAlloc();

        if (udesc->entry_number == TLS_OUT_OF_INDEXES)
        {
            return -LINUX_ESRCH;
        }
    }

    pSegmentDescriptor =
        (linux_user_desc_t *) malloc(sizeof(linux_user_desc_t));
    memcpy(pSegmentDescriptor, udesc, sizeof(linux_user_desc_t));

    printf("sys_set_thread_area: segment %d: base = 0x%p, limit = 0x%p\n",
           pSegmentDescriptor->entry_number,
           pSegmentDescriptor->base_addr,
           pSegmentDescriptor->limit);

    TlsSetValue(udesc->entry_number, pSegmentDescriptor);

    return 0;
}
