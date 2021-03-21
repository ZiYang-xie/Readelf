/* Created By Xzy */
/* March-21-2021 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//static const char* PATH = "./main";
static unsigned char buff[256];

static const char* entry_list[] = {
    " Entry point address:\t\t\t",
    " Start of program headers:\t\t",
    " Start of section headers:\t\t",
    " Flags:\t\t\t\t\t",
    " Size of this header:\t\t\t",
    " Size of program headers:\t\t",
    " Number of program headers:\t\t",
    " Size of section headers:\t\t",
    " Number of section headers:\t\t",
    " Section header string table index:\t"
};
static const char* pt_args_list[] = {
    "0x%x\n",
    "%d (bytes into file)\n",
    "%d (bytes into file)\n",
    "0x%x\n",
    "%d (bytes)\n",
    "%d (bytes)\n",
    "%d\n",
    "%d (bytes)\n",
    "%d\n",
    "%d\n"
};

// read into buff
void readBuf(FILE* fp)
{
    for(int i = 0; i < 64; ++i)
    {
        buff[i] = fgetc(fp);
        //printf("%2x\n", buff[i]);
    }
}

// show the help info
void showHelp()
{
    printf("Usage: readelf <option(s)> elf-file(s)\n");
    printf(" Display information about the contents of ELF format files\n");
    printf(" Options are:\n");
    printf("\t -h --file-header \t Display the ELF file header\n");
    printf("\t -H --help \t Display this information\n");
}

// read_util can deal with both 32 or 64 bits and little or big endian
int read_util(int base, int offset, int limit)
{
    const int flag = buff[5] == 1 ? 1 : -1;
    const int end = buff[5] - 1;
    const int start = base + offset - end + limit*end;
    int buffnum = 0;
    
    for(int i = 0; i < limit; ++i)
        buffnum += buff[start + i*(flag)] << i*8;
    buffnum = buffnum & 0xffffffff;
    return buffnum;
}

// get magic num (e_ident: 16bytes)
void getMagic()
{
    
    printf(" Magic:\t\t");
    for(int i = 0; i < 16; ++i)
        printf("%.2x ", buff[i]);
    printf("\n");
}

// get class (e_ident[EI_CLASS]: 1byte)
void getClass()
{
    printf(" Class:");
    printf("\t\t\t\t\t");
    if(buff[4] == 1)
        printf("ELF32");
    else if(buff[4] == 2)
        printf("ELF64");
    else // ERROR
    {
        printf("Error with the Undefined EI_Class\n");
        exit(-1);
    }
    printf("\n");
}

// get data (e_ident[EI_DATA]: 1byte)
void getData()
{
    printf(" Data:");
    printf("\t\t\t\t\t");
    if(buff[5] == 1)
        printf("little endian");
    else if(buff[5] == 2)
        printf("big endian");
    else // ERROR
    {
        printf("Error with the Undefined EI_Data\n");
        exit(-1);
    }
    printf("\n");
}


// get version (e_ident[EI_VERSION]: 1byte)
// Set to 1 for the original and current version of ELF.
void getVersion()
{
    int len = 2;
    printf(" Version:");
    printf("\t\t\t\t");
    if(buff[6] == 1)
        printf("1 (current)");
    else // ERROR
    {
        printf("Error with the Undefined EI_VERSION\n");
        exit(-1);
    }
    printf("\n");
}

// get OS (e_ident[EI_OSABI]: 1byte)
void getOS()
{
    printf(" OS/ABI:");
    printf("\t\t\t\t");
    char *OS[] = {
        "System V", 
        "HP-UX", 
        "NetBSD",
        "Linux",
        "GNU Hurd",
        "Solaris",
        "AIX",
        "IRIX",
        "FreeBSD",
        "Tru64",
        "Novell Modesto",
        "OpenBSD",
        "OpenVMS",
        "NonStop Kernel",
        "AROS",
        "Fenix OS",
        "CloudABI",
        "Stratus Technologies OpenVOS"
    };
    const int buff_num = buff[7];
    if(buff_num >= 0 && buff_num <= 12)
        printf("%s", OS[buff_num]);
    else // ERROR
    {
        printf("Error with the Undefined EI_OSABI\n");
        exit(-1);
    }
    printf("\n");
}

// get ABI Version (e_ident[EI_ABIVERSION]: 1byte)
void getABIV()
{
    printf(" ABI Version:");
    printf("\t\t\t\t");
    printf("%d", buff[8]);
    printf("\n");
}

// e_type
void getType()
{
    printf(" Type:");
    printf("\t\t\t\t\t");
    const char *TYPE[] = {
        "NONE (Unknown file)",
        "REL (Relocatable file)",
        "EXEC (Executable file)",
        "DYN (Shared object file)",
        "CORE (Core file)",
    };
    const int buff_num = read_util(16, 0, 2);
    
    if(buff_num >= 0 && buff_num <= 4)
        printf("%s", TYPE[buff_num]);
    else if(buff_num == 0xfe00)
        printf("LOOS");
    else if(buff_num == 0xfefe)
        printf("HIOS");
    else if(buff_num == 0xff00)
        printf("LOPROC (Specific Processor file)");
    else if(buff_num == 0xffff)
        printf("HIPROC (Specific Processor file)");
    else // ERROR
    {
        printf("Error with the Undefined e_type: %.4x\n", buff_num);
        exit(-1);
    }
    printf("\n");
}

// e_machine
void getMachine()
{
    printf(" Machine:");
    printf("\t\t\t\t");
    // Part of the Machine //TODO
    const char *MACHINE[] = {
        "No specific instruction set",
        "AT&T WE 32100",
        "SPARC",
        "x86",
        "Motorola 68000 (M68k)",
        "Motorola 88000 (M88k)",
        "Intel MCU",
        "Intel 80860",
        "MIPS",
        "IBM_System/370",
        "MIPS RS3000 Little-endian",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Hewlett-Packard PA-RISC",
        "Reserved",
        "Fujitsu VPP500",
        "Enhanced instruction set SPARC",
        "Intel 80960",
        "PowerPC",
        "PowerPC (64-bit)",
        "S390, including S390x"
    };
    const int buff_num = read_util(18, 0, 2);

    if(buff_num >= 0 && buff_num <= 22)
        printf("%s", MACHINE[buff_num]);
    else if(buff_num == 0x28)
        printf("ARM");
    else if(buff_num == 0x2A)
        printf("SuperH");
    else if(buff_num == 0x3E)
        printf("Advanced Micro Devices X86-64");
    else if(buff_num == 0x8C)
        printf("TMS320C6000 Family");
    else if(buff_num == 0xB7)
        printf("ARM 64-bits");
    else if(buff_num == 0xf3)
        printf("RISC-V");
    else if(buff_num == 0xf7)
        printf("Berkeley Packet Filter");
    else if(buff_num == 0x101)
        printf("WDC 65C816");
    else // ERROR
    {
        printf("Error with the Undefined e_machine: %.4x\n", buff_num);
        exit(-1);
    }
    printf("\n");
}

// e_version
// Set to 1 for the original version of ELF.
void getE_Version()
{
    printf(" Version:");
    printf("\t\t\t\t");
    if(buff[20] != 0x1) // error
    {
        printf("Error with the Undefined e_version: %.2x\n", buff[20]);
        exit(-1);
    }
    printf("0x%x", buff[20]);
    printf("\n");
}

void Decode(const char* entry,const char* pt_args, int base, int sz)
{
    printf("%s", entry);
    const int buffnum = read_util(base, 0, sz);
    printf(pt_args, buffnum);
}

void DecodeHelper(const char* entry_list[],const char* pt_args_list[], int* sz, int base)
{
    //assert len(entry_list == pt_args_list) = 10
    for(int i = 0; i < 10; ++i)
    {
        Decode(entry_list[i], pt_args_list[i], base, sz[i]);
        base += sz[i];
    }
}


int main(int argc, char *argv[])
{
    if(argc == 3 && !strcmp(argv[1], "-h"))
    {
        const char* PATH = argv[2];
        FILE* fp = fopen(PATH, "r");

        readBuf(fp);
        printf("ELF Header\n");

        getMagic();
        getClass();
        getData();
        getVersion();
        getOS();
        getABIV();
        getType();
        getMachine();
        getE_Version();

        int bitSz = 4 * buff[4];
        int sz[] = {
            bitSz, bitSz, bitSz,
            4, 2, 2, 2, 2, 2, 2
        };
        DecodeHelper(entry_list, pt_args_list, sz, 24);

        fclose(fp);
    }
    else if(!strcmp(argv[1], "-H") || !strcmp(argv[1], "--help"))
    {
        showHelp();
        return -1;
    }
    else 
        showHelp();
}