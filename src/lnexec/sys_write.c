#include <windef.h>

#include "sys_proto.h"
#include "linux_errno.h"

#include <iofuncs.h>
#include <rtlfuncs.h>

HANDLE TranslateFile(int fd)
{
    switch (fd)
    {
    case 0:
        return NtCurrentPeb()->ProcessParameters->StandardInput;
    case 1:
        return NtCurrentPeb()->ProcessParameters->StandardOutput;
    case 2:
        return NtCurrentPeb()->ProcessParameters->StandardError;

    default:
        return NULL;
    }
}

int sys_writev(int fd, 
               linux_iovec_t *iov,
               size_t iovcnt)
{
    HANDLE hFile = TranslateFile((int)fd);
    NTSTATUS Status;
    IO_STATUS_BLOCK Iosb;
    INT CurrentBuffer;
    SSIZE_T TotalBytes;

    if (!hFile)
    {
        return -LINUX_EBADF;
    }

    // FIXME: Fallback

    TotalBytes = 0;

    for (CurrentBuffer = 0; CurrentBuffer < iovcnt; CurrentBuffer++)
    {
        Status = NtWriteFile(hFile,
                             NULL,
                             NULL,
                             NULL,
                             &Iosb,
                             iov[CurrentBuffer].iov_base,
                             iov[CurrentBuffer].iov_len,
                             NULL,
                             NULL);

        if (NT_SUCCESS(Status))
        {
            TotalBytes += Iosb.Information;
        }
        else
        {
            return -1;
        }
    }

    return TotalBytes;
}
