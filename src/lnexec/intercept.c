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
    case 0xc7:
        if (PC[2] == 0x00)
        {
            // mov gs:[eax], imm32

            Address = GSTranslateAddress((PVOID) pContext->Eax);
            *(UINT *)Address = *(UINT *)&PC[3];
            pContext->Eip += 7;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
        break;

    case 0x83:
        if (PC[2] == 0x3d)
        {
            // cmp gs:[imm32], imm8

            BYTE A, B;

            Address = GSTranslateAddress(*(PVOID *) &PC[3]);
            A = *(PBYTE)Address;
            B = PC[7];

            pContext->EFlags &=~ (0x01 | 0x40 | 0x80);
            pContext->EFlags |= (B > A) * 0x01;
            pContext->EFlags |= (B == A) * 0x40;
            pContext->EFlags |= (B > A) * 0x01;

            pContext->Eip += 8;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
        break;

    case 0x8b:
        if (PC[2] == 0x3a)
        {
            // mov         edi,dword ptr gs:[edx]

            Address = GSTranslateAddress((PVOID) pContext->Edx);
            pContext->Edi = *(UINT *)Address;
            pContext->Eip += 3;

            return EXCEPTION_CONTINUE_EXECUTION;
        }

        if (PC[2] == 0x00)
        {
            // mov         eax,dword ptr gs:[eax]  

            Address = GSTranslateAddress((PVOID) pContext->Eax);
            pContext->Eax = *(UINT *)Address;
            pContext->Eip += 3;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
        break;

    default:
        printf("Unexpected GS operation. Code dump around PC:\n");
        DumpAroundPC(PC);
        system("pause");
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
