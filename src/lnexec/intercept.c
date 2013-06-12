#include <windef.h>
#include <rtlfuncs.h>

static LONG NTAPI ExceptionHandler(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    return EXCEPTION_CONTINUE_SEARCH;
}

void InitSyscallInterceptor()
{
    RtlAddVectoredExceptionHandler(1, ExceptionHandler);
}
