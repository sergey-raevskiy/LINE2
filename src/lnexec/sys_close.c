#include <windef.h>

#include "linux_errno.h"

int sys_close(int fd)
{
    // Do not close any fd's at the moment.

    return 0;
}
