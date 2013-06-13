#include <atlstr.h>
#include <stdio.h>

#include "..\ElfLoader\ElfFile.h"
#include "..\Common\Exception.h"

#include "lnexec.h"

void eprintf(const char *str, ...)
{
    va_list va;
    va_start(va, str);
    vfprintf(stderr, str, va);
    va_end(va);
}

typedef struct {
    DWORD Type;
    DWORD Val;
} linux_auxv_t;

#define AT_NULL     0       /* End of vector */
#define AT_IGNORE   1       /* Entry should be ignored */
#define AT_EXECFD   2       /* File descriptor of program */
#define AT_PHDR     3       /* Program headers for program */
#define AT_PHENT    4       /* Size of program header entry */
#define AT_PHNUM    5       /* Number of program headers */
#define AT_PAGESZ   6       /* System page size */
#define AT_BASE     7       /* Base address of interpreter */
#define AT_FLAGS    8       /* Flags */
#define AT_ENTRY    9       /* Entry point of program */
#define AT_NOTELF   10      /* Program is not ELF */
#define AT_UID      11      /* Real uid */
#define AT_EUID     12      /* Effective uid */
#define AT_GID      13      /* Real gid */
#define AT_EGID     14      /* Effective gid */
#define AT_CLKTCK   17      /* Frequency of times() */

void start(void *entry, Elf32_Phdr *phdr, UINT phnum)
{
    __asm
    {
        mov eax, 0
        push eax
        push eax

        mov eax, phdr
        push eax
        mov eax, AT_PHDR
        push eax
        mov eax, phnum
        push eax
        mov eax, AT_PHNUM
        push eax
        mov eax, 25
        push eax
        mov eax, esp
        push eax

        xor eax, eax

        push eax // argc
        push eax // last argv
        push eax // last envp

        jmp entry
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        eprintf("Usage: %s ELF <args>", argv[0]);
        return 1;
    }

    try
    {
        CString path = CA2W(argv[1]);
        ElfFile executable(path);

        Elf32_Phdr *ph;
        UINT PHnum;

        executable.GetPhdr(ph, PHnum);

        executable.Map(GetCurrentProcess());

        SetInitialBreak(executable.GetBreak());
        InitSyscallInterceptor();

        start(executable.GetEntryPoint(), ph, PHnum);
    }
    catch (const Exception & ex)
    {
        eprintf("Fail: %s", CW2A(ex.GetDescription()));
        return 1;
    }
}
