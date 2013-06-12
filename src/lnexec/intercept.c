#include <windef.h>
#include <rtlfuncs.h>

#include "lnexec.h"

static LONG TryHandleInt80(PCONTEXT pContext)
{
    PBYTE PC = (PBYTE)pContext->Eip;

    if (PC[0] == 0xcd && PC[1] == 0x80)
    {
        // This is INT 80h, so pass control to the syscall handler.

        // push EIP to stack
        ((PUINT)pContext->Esp)--;
        *(PUINT)pContext->Esp = pContext->Eip + 2; // skip the int 80h

        pContext->Eip = (UINT)(HandleSyscall);

        return EXCEPTION_CONTINUE_EXECUTION;
    }
    else
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }
}

static LONG NTAPI ExceptionHandler(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    LONG Result = EXCEPTION_CONTINUE_SEARCH;

    Result = TryHandleInt80(ExceptionInfo->ContextRecord);

    return Result;
}

void InitSyscallInterceptor()
{
    RtlAddVectoredExceptionHandler(1, ExceptionHandler);
}
