#pragma once

#include <windef.h>
#include <vector>
#include <map>

/* 32-bit ELF base types. */
typedef unsigned int Elf32_Addr;
typedef unsigned short Elf32_Half;
typedef unsigned int Elf32_Off;
typedef signed int Elf32_Sword;
typedef unsigned int Elf32_Word;

#define EI_NIDENT 16

typedef struct {
    unsigned char   e_ident[EI_NIDENT];
    Elf32_Half      e_type;
    Elf32_Half      e_machine;
    Elf32_Word      e_version;
    Elf32_Addr      e_entry;
    Elf32_Off       e_phoff;
    Elf32_Off       e_shoff;
    Elf32_Word      e_flags;
    Elf32_Half      e_ehsize;
    Elf32_Half      e_phentsize;
    Elf32_Half      e_phnum;
    Elf32_Half      e_shentsize;
    Elf32_Half      e_shnum;
    Elf32_Half      e_shtrndx;
} Elf32_Hdr;

/* sh_type */
#define SHT_NULL         0
#define SHT_PROGBIT      1
#define SHT_SYMTAB       2
#define SHT_STRTAB       3
#define SHT_RELA         4
#define SHT_HASH         5
#define SHT_DYNAMIC      6
#define SHT_NOTE         7
#define SHT_NOBITS       8
#define SHT_REL          9
#define SHT_SHLIB        10
#define SHT_DYNSYM       11
#define SHT_NUM          12
#define SHT_LOPROC       0x70000000
#define SHT_HIPROC       0x7fffffff
#define SHT_LOUSER       0x80000000
#define SHT_HIUSER       0xffffffff

typedef struct {
    Elf32_Word      sh_name;
    Elf32_Word      sh_type;
    Elf32_Word      sh_flags;
    Elf32_Addr      sh_addr;
    Elf32_Off       sh_offset;
    Elf32_Word      sh_size;
    Elf32_Word      sh_link;
    Elf32_Word      sh_info;
    Elf32_Word      sh_addralign;
    Elf32_Word      sh_entsize;
} Elf32_Shdr;

class ElfFile
{
public:
    ElfFile(LPCWSTR FileName);
    ~ElfFile();

private:
    ElfFile(const ElfFile &);

    HANDLE m_File;
    Elf32_Hdr m_Header;

    std::vector<Elf32_Shdr> m_SectionHeaders;
    std::vector<char> m_StringTable;

    void ReadHeader();
    void ReadSectionHeaders();
    void ReadStringTable();
};
