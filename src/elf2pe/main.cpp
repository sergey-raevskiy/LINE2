#include <stdio.h>
#include <stdarg.h>

void eprintf(wchar_t *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    vfwprintf(stderr, fmt, va);
    va_end(va);
}

int wmain(int argc, wchar_t **argv)
{
    if (argc < 3)
    {
        eprintf(L"Usage: elf2pe <input file> <output file>\n");
        return 1;
    }

    return 0;
}
