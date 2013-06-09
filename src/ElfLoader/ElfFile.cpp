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

static void ReadFromFile(HANDLE File,
                         PVOID Buffer,
                         SIZE_T Offset,
                         SIZE_T Length)
{
    IO_STATUS_BLOCK Iosb;
    LARGE_INTEGER LOffset;

    LOffset.QuadPart = LONGLONG(Offset);

    NT_ERR_E(NtReadFile(File,
                        NULL,
                        NULL,
                        NULL,
                        &Iosb,
                        Buffer,
                        Length,
                        &LOffset,
                        NULL));
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

ElfFile::ElfFile(LPCWSTR FileName)
    : m_File(OpenElfFile(FileName))
{
    ReadHeader();
    ValidateHeader(&m_Header);

    ReadSectionHeaders();
    ReadStringTable();
    ReadProgramHeaders();
}

ElfFile::~ElfFile()
{
    NTSTATUS Status = NtClose(m_File);
    ASSERT(NT_SUCCESS(Status));
}

void ElfFile::ReadHeader()
{
    ReadFromFile(m_File, &m_Header, 0, sizeof(Elf32_Hdr));
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

        ReadFromFile(m_File,
                     &SectionHeader,
                     CurrentSectionOffset,
                     sizeof(Elf32_Shdr));

        m_SectionHeaders.push_back(SectionHeader);
    }
}

static std::vector<Elf32_Shdr>::const_iterator
FindStringTable(const std::vector<Elf32_Shdr> & sections)
{
    for (std::vector<Elf32_Shdr>::const_iterator It = sections.begin();
                                                 It != sections.end();
                                                 It++)
    {
        if (It->sh_type == SHT_STRTAB)
        {
            return It;
        }
    }

    return sections.end();
}

void ElfFile::ReadStringTable()
{
    m_StringTable.clear();

    // TODO: May ELF contain more than one STRTAB section?

    std::vector<Elf32_Shdr>::const_iterator It =
        FindStringTable(m_SectionHeaders);

    if (It == m_SectionHeaders.end())
        return;

    m_StringTable.resize(It->sh_size);

    ReadFromFile(m_File,
                 &m_StringTable[0],
                 It->sh_offset,
                 It->sh_size);
}

void ElfFile::ReadProgramHeaders()
{
    int CurrentHeader;
    Elf32_Off CurrentHeaderOffset;

    m_ProgramHeaders.clear();

    for (CurrentHeader = 0, CurrentHeaderOffset = m_Header.e_phoff;
         CurrentHeader < m_Header.e_phnum;
         CurrentHeader++, CurrentHeaderOffset += m_Header.e_phentsize)
    {
        Elf32_Phdr ProgramHeader;

        ReadFromFile(m_File,
                     &ProgramHeader,
                     CurrentHeaderOffset,
                     sizeof(Elf32_Phdr));

        m_ProgramHeaders.push_back(ProgramHeader);
    }
}
