#include "ElfFile.h"

#include "..\Common\NtException.h"
#include <iofuncs.h>
#include <rtlfuncs.h>
#include <obfuncs.h>

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

static void ValidateHeader(Elf32_Hdr *Header)
{
    BOOLEAN ValidMagick = Header->e_ident[0] == 0x7f &&
                          Header->e_ident[1] == 'E'  &&
                          Header->e_ident[2] == 'L'  &&
                          Header->e_ident[3] == 'F';

    if (!ValidMagick)
    {
        throw Exception(L"Invalid ELF magick");
    }
}

static void ReadSectionHeader(HANDLE File,
                              Elf32_Shdr *SectionHeader,
                              Elf32_Off Offset)
{
    LARGE_INTEGER LOffset;
    IO_STATUS_BLOCK Iosb;

    LOffset.QuadPart = LONGLONG(Offset);

    NT_ERR_E(NtReadFile(File,
                        NULL,
                        NULL,
                        NULL,
                        &Iosb,
                        SectionHeader,
                        sizeof(Elf32_Shdr),
                        &LOffset,
                        NULL));
}

ElfFile::ElfFile(LPCWSTR FileName)
    : m_File(OpenElfFile(FileName))
{
    ReadHeader();
    ValidateHeader(&m_Header);

    ReadSectionHeaders();
}

ElfFile::~ElfFile()
{
    NTSTATUS Status = NtClose(m_File);
    ASSERT(NT_SUCCESS(Status));
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

void ElfFile::ReadSectionHeaders()
{
    int CurrentSection;
    Elf32_Off CurrentSectionOffset;

    m_SectionHeaders.clear();

    for (CurrentSection = 0, CurrentSectionOffset = m_Header.e_shoff;
         CurrentSection < m_Header.e_shnum;
         CurrentSection++, CurrentSectionOffset += m_Header.e_shentsize)
    {
        Elf32_Shdr SectionHeader;

        ReadSectionHeader(m_File, &SectionHeader, CurrentSectionOffset);
        m_SectionHeaders.push_back(SectionHeader);
    }
}
