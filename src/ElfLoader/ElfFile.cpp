#include "ElfFile.h"

#include "..\Common\NtException.h"
#include <iofuncs.h>
#include <rtlfuncs.h>

static HANDLE OpenElfFile(LPCWSTR FileName)
{
    OBJECT_ATTRIBUTES FileObjectAttributes;
    UNICODE_STRING UsFileName;
    IO_STATUS_BLOCK Iosb;
    HANDLE FileHandle;

    RtlInitUnicodeString(&UsFileName, FileName);

    InitializeObjectAttributes(&FileObjectAttributes,
                               &UsFileName,
                               0,
                               NULL,
                               NULL);

    NT_ERR_E(NtOpenFile(&FileHandle,
                        FILE_READ_DATA | FILE_EXECUTE,
                        &FileObjectAttributes,
                        &Iosb,
                        FILE_SHARE_READ | FILE_SHARE_DELETE,
                        FILE_NON_DIRECTORY_FILE));

    return FileHandle;
}

ElfFile::ElfFile(LPCWSTR FileName)
    : m_File(OpenElfFile(FileName))
{
    ReadHeader();
}

void ElfFile::ReadHeader()
{
    IO_STATUS_BLOCK Iosb;
    LARGE_INTEGER Offset;

    Offset.QuadPart = 0;

    NT_ERR_E(NtReadFile(m_File,
                        NULL,
                        NULL,
                        NULL,
                        &Iosb,
                        &m_Header,
                        sizeof(Elf32_Hdr),
                        &Offset,
                        NULL));
}
