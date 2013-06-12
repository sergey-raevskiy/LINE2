#include "sys_proto.h"
#include "linux_errno.h"

#include <windows.h>
#include <rtlfuncs.h>

int sys_time(linux_time_t *t)
{
    ULONG Result;
    LARGE_INTEGER Time;
    FILETIME Ft;

    GetSystemTimeAsFileTime(&Ft);

    Time.LowPart = Ft.dwLowDateTime;
    Time.HighPart = Ft.dwHighDateTime;

    RtlSystemTimeToLocalTime(&Time, &Time);
    RtlTimeToSecondsSince1970(&Time, &Result);

    if (t)
    {
        *t = Result;
    }

    return Result;
}
