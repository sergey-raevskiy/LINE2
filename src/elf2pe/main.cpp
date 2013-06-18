#include <stdio.h>
#include <stdarg.h>
#include <atlstr.h>
#include <atlfile.h>

void eprintf(wchar_t *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    vfwprintf(stderr, fmt, va);
    va_end(va);
}

int ProcessFile(LPCWSTR inFileName, LPCWSTR outFileName)
{
    CAtlFile inFile;
    CAtlFile outFile;

    inFile.Create(inFileName,
                  FILE_READ_DATA,
                  FILE_SHARE_READ | FILE_SHARE_DELETE,
                  OPEN_EXISTING);

    outFile.Create(outFileName,
                   FILE_WRITE_DATA,
                   0,
                   CREATE_ALWAYS);

    return 0;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc < 3)
    {
        eprintf(L"Usage: elf2pe <input file> <output file>\n");
        return 1;
    }

    return ProcessFile(argv[1], argv[2]);
}
