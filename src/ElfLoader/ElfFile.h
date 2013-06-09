#pragma once

#include <windef.h>

class ElfFile
{
public:
    ElfFile(LPCWSTR FileName);

private:
    ElfFile(const ElfFile &);

    HANDLE m_File;
};
