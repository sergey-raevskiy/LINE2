#include <windef.h>
#include <rtlfuncs.h>
#include <stdio.h>

#include "lnexec.h"
#include "gs_support.h"

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

static void DumpAroundPC(PBYTE PC)
{
    PBYTE Begin = PC - 10,
          End = PC + 10,
          Current;

    for (Current = Begin; Current < End; Current++)
    {
        if (Current == PC)
        {
            printf("@");
        }

        printf("%02x ", *Current);
    }
}

static LONG HandleGsIstruction(PCONTEXT pContext)
{
    PBYTE PC = (PBYTE)pContext->Eip;
    PVOID Address;

    switch (PC[1])
    {
    case 0xa3:

        // mov gs:[x], eax

        Address = *(PVOID *) &PC[2];
        Address = GSTranslateAddress(Address);

        *(UINT *) Address = pContext->Eax;
        pContext->Eip += 6;

        return EXCEPTION_CONTINUE_EXECUTION;

        break;
    default:
        printf("Unexpected GS operation. Code dump around PC:\n");
        DumpAroundPC(PC);
        exit(1);
        break;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

static LONG TryHandleGS(PCONTEXT pContext)
{
    PBYTE PC = (PBYTE)pContext->Eip;

    switch (PC[0])
    {
    case 0x8e:
        if (PC[1] == 0xe8)
        {
            // mov gs, ax

            GSLoad(pContext->Eax & 0xffff);
            pContext->Eip += 2;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
        break;
    case 0x65:
        return HandleGsIstruction(pContext);
        break;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

static LONG NTAPI ExceptionHandler(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    LONG Result = EXCEPTION_CONTINUE_SEARCH;

    Result = TryHandleInt80(ExceptionInfo->ContextRecord);

    if (Result == EXCEPTION_CONTINUE_SEARCH)
    {
        Result = TryHandleGS(ExceptionInfo->ContextRecord);
    }

    return Result;
}

void InitSyscallInterceptor()
{
    RtlAddVectoredExceptionHandler(1, ExceptionHandler);
}
