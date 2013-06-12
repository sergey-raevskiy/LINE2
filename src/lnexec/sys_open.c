#include <stdio.h>

#include "linux_errno.h"

int sys_open(const char *name,
             int flags,
             int mode)
{
    printf("sys_open(\"%s\", %x, %x): stub\n", name, flags, mode);

    return -LINUX_ENOSYS;
}
