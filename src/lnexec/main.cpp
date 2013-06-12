#include <atlstr.h>
#include <stdio.h>

#include "..\ElfLoader\ElfFile.h"
#include "..\Common\Exception.h"

void eprintf(const char *str, ...)
{
    va_list va;
    va_start(va, str);
    vfprintf(stderr, str, va);
    va_end(va);
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

        executable.Map(GetCurrentProcess());
    }
    catch (const Exception & ex)
    {
        eprintf("Fail: %s", CW2A(ex.GetDescription()));
        return 1;
    }
}
