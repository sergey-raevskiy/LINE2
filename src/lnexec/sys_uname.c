#include <string.h>

#include "sys_proto.h"

int sys_uname(linux_new_utsname_t *utsname)
{
    strncpy(utsname->sysname, "Linux", __NEW_UTS_LEN);
    strncpy(utsname->nodename, "LINE2", __NEW_UTS_LEN);
    strncpy(utsname->release, "LINE2", __NEW_UTS_LEN);
    strncpy(utsname->version, "LINE2", __NEW_UTS_LEN);
    strncpy(utsname->machine, "x86", __NEW_UTS_LEN);
    strncpy(utsname->domainname, "example.com", __NEW_UTS_LEN);

    return 0;
}
