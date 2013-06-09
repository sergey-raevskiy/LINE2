#pragma once

#include <windef.h>

class ElfFile
{
public:
    ElfFile(LPCWSTR FileName);

private:
    HANDLE m_File;
};
