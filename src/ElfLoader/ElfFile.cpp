#include "ElfFile.h"

#include "..\Common\NtException.h"
#include "..\Common\NtHandle.h"
#include <iofuncs.h>
#include <rtlfuncs.h>
#include <obfuncs.h>
#include <mmfuncs.h>
#include <intsafe.h>

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

void ElfFile::GetImageBoundaries(SIZE_T & Min, SIZE_T & Max)
{
    Min = SIZE_T_MAX;
    Max = 0;

    for (std::vector<Elf32_Phdr>::const_iterator It = m_ProgramHeaders.begin();
                                                 It != m_ProgramHeaders.end();
                                                 It++)
    {
        if (It->p_type != PT_LOAD)
            continue;

        if (It->p_vaddr < Min)
        {
            Min = It->p_vaddr;
        }

        SIZE_T End = It->p_vaddr + It->p_memsz;

        if (End > Max)
        {
            Max = End;
        }
    }
}

void ElfFile::Map(HANDLE hProcess)
{
    NtHandle hSection;

    SIZE_T Min, Max;

    GetImageBoundaries(Min, Max);

    SIZE_T MappingBase = Min & 0xffff0000;
    SIZE_T MappingSize = (Max - Min) + (Min & 0xffff);

    LARGE_INTEGER SectionSize;
    SectionSize.QuadPart = LONGLONG(MappingSize);

    NT_ERR_E(NtCreateSection(&hSection,
                             SECTION_MAP_READ | SECTION_MAP_EXECUTE |
                             SECTION_MAP_WRITE | SECTION_QUERY,
                             NULL,
                             &SectionSize,
                             PAGE_EXECUTE_READWRITE,
                             SEC_COMMIT, // TODO: WTF
                             NULL));

    NT_ERR_E(NtMapViewOfSection(hSection,
                                hProcess,
                                (PVOID *)&MappingBase,
                                0,
                                0,
                                NULL,
                                &MappingSize,
                                ViewShare,
                                0,
                                PAGE_EXECUTE_READWRITE));

    ULONG OldProtection;

    /*NT_ERR_E(NtProtectVirtualMemory(hProcess,
                                    (PVOID *)&MappingBase,
                                    &MappingSize,
                                    PAGE_NOACCESS,
                                    &OldProtection));*/

    for (std::vector<Elf32_Phdr>::const_iterator It = m_ProgramHeaders.begin();
                                                 It != m_ProgramHeaders.end();
                                                 It++)
    {
        if (It->p_type != PT_LOAD)
            continue;

        PVOID BaseAddress = PVOID(It->p_vaddr);

        ReadFromFile(m_File, BaseAddress, It->p_offset, It->p_filesz);

        ULONG Protection, OldProtection;

        switch (It->p_flags)
        {
        case PF_R:
            Protection = PAGE_READONLY;
        case PF_R | PF_W:
            Protection = PAGE_WRITECOPY;
        case PF_R | PF_X:
            Protection = PAGE_EXECUTE_READ;
        default:
            ASSERT(FALSE);
        }

        SIZE_T MemorySize = It->p_memsz;

        NT_ERR_E(NtProtectVirtualMemory(hProcess,
                                        &BaseAddress,
                                        &MemorySize,
                                        Protection,
                                        &OldProtection));

        if (It->p_memsz > It->p_filesz && It->p_flags & PF_W)
        {
            //memset((BYTE *)BaseAddress + It->p_filesz, 0, It->p_memsz - It->p_filesz);
        }
    }
}
