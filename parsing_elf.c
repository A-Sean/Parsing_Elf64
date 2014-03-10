#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#define EI_NIDENT 16
typedef struct
{
        unsigned char _ident[EI_NIDENT];
        unsigned short int e_type;
        unsigned short int e_machine;
        unsigned int e_version;
        unsigned long int e_entry;
        unsigned long int e_phoff;
        unsigned long int e_shoff;
        unsigned int e_flags;
        unsigned short int e_ehsize;
        unsigned short int e_phentsize;
        unsigned short int e_phnum;
        unsigned short int e_shentsize;
        unsigned short int e_shnum;
        unsigned short int e_shstrndx;
}Elf64_Ehdr;

typedef struct
{
        unsigned int sh_name;
        unsigned int sh_type;
        unsigned long int sh_flags;
        unsigned long int sh_addr;
        unsigned long int sh_offset;
        unsigned long int sh_size;
        unsigned int sh_link;
        unsigned int sh_info;
        unsigned long int sh_addralign;
        unsigned long int sh_entsize;
}Elf64_Shdr;

typedef struct
{
        unsigned int st_name;
        unsigned char st_info;
        unsigned char st_other;
        unsigned short int st_shndx;
        unsigned long int st_value;
        unsigned long int st_size;
}Elf64_Sym;



void print_elf_header_info(Elf64_Ehdr*);
void print_section_info(Elf64_Shdr*);

int main(int argc,char *argv[])
{
        if(argc!=2)
        {
                printf("Command Line parameter missing.Exitting.Enter an elf filename at the command line. \"./a.out elf_filename\"");
                exit(EXIT_FAILURE);
        }

        FILE *ptr;
        ptr = fopen(argv[1],"rb");
        if(ptr == NULL)
        {
                printf("elf file not found. exitting\n");
                exit(EXIT_FAILURE);
        }
        //reading the header
        Elf64_Ehdr h;
        int i;
        fread(&h,1,sizeof(h),ptr);
        //displaying the header info
        printf("\t\t\t\tPRINTING ELF HEADER INFO:\n\n");
        print_elf_header_info(&h);
        //reading the sections
        rewind(ptr);
        fseek(ptr,h.e_shoff,SEEK_CUR);
        Elf64_Shdr sh;
        Elf64_Shdr sec_hdr[h.e_shnum];
        //printing the section header info
        printf("\t\t\t\tSECTION HEADER INFO:\n\n");
        printf("Name\t Type\t Flags\t Addr\t Offset\t Size\t Link\t Info\t Addralign   Entsize\n\n");
        unsigned int symtab_ndx;
        unsigned int strtab_ndx;
        for(i=0;i<h.e_shnum;i++)
        {
                fread(&sec_hdr[i],1,sizeof(sh),ptr);
                //collecting information for the symbol table;
                if(sec_hdr[i].sh_type == 2)
                {
                        //2 is the type for .symtab
                        symtab_ndx = i;
                }
                if(sec_hdr[i].sh_type == 3)
                {
                        //3 is the type for .strtab
                        strtab_ndx = i;
                }
                print_section_info(&sec_hdr[i]);
                printf("\n");
        }
        printf("\nThe section index for the symbol table is %d\n\n",symtab_ndx);
        printf("\nThe section index for the strtab is %d\n\n",strtab_ndx);
        printf("\n\nThe names of the sections are:\n");
        char* buf = (char*)malloc(sec_hdr[h.e_shstrndx].sh_size);//buf will contain the contents of .shstrtab
        rewind(ptr);
         fseek(ptr,sec_hdr[h.e_shstrndx].sh_offset,SEEK_CUR);
        fread(buf,1,sec_hdr[h.e_shstrndx].sh_size,ptr);
        int start_index;
        for(i = 0;i<h.e_shnum;i++)
        {
                start_index = sec_hdr[i].sh_name;
                if(buf[start_index] == '\0')
                {
                        continue;
                }
                else
                {
                        while(buf[start_index]!='\0')
                        {
                                printf("%c",buf[start_index]);
                                start_index++;
                        }
                }
                printf("\n");
        }
        rewind(ptr);
        //dumping the symbol table
        //printing the .symtab info
        unsigned symtab_ent = sec_hdr[symtab_ndx].sh_size / sec_hdr[symtab_ndx].sh_entsize;
        printf("\n\nNumber of symbol table entries = %d \n\n",symtab_ent);
        Elf64_Sym sym[symtab_ent];//array of structures for storing the symbol table
        Elf64_Sym s;
        fseek(ptr,sec_hdr[symtab_ndx].sh_offset,SEEK_CUR);
        for(i=0;i<symtab_ent;i++)
        {
                fread(&sym[i],1,sizeof(s),ptr);
        }
        //displaying the symbol information
        printf("\t\t\tTHE SYMBOL TABLE INFORMATION :\n\n");
        printf("Num:\t Name\t Info\t Other\t STndx\t Value\t Size\n\n");
        for(i=0;i<symtab_ent;i++)
        {
                printf("%d:\t ",i);
                printf("%d\t ",sym[i].st_name);
                printf("%x\t ",sym[i].st_info);
                printf("%x\t ",sym[i].st_other);
                printf("%d\t ",sym[i].st_shndx);
                printf("%x\t ",sym[i].st_value);
                printf("%d\n",sym[i].st_size);
        }
        //printing the names of the symbols
        printf("\n\nThe names of the entries in the symbol table are:\n\n");
        rewind(ptr);
        fseek(ptr,sec_hdr[strtab_ndx].sh_offset,SEEK_CUR);
        char *strtab_buf = (char*)malloc(sec_hdr[strtab_ndx].sh_size);
         fread(strtab_buf,1,sec_hdr[strtab_ndx].sh_size,ptr);
        for(i=0;i<symtab_ent;i++)
        {
                if(sym[i].st_name != 0)
                {
                        start_index = sym[i].st_name;
                        if(strtab_buf[start_index] == '\0')
                        {
                                continue;
                        }
                         else
                        {
                                while(strtab_buf[start_index]!='\0')
                                {
                                        printf("%c",strtab_buf[start_index]);
                                         start_index++;
                                }
                        }
                        printf("\n");
                }
        }
        fclose(ptr);
        return 0;

}

void print_elf_header_info(Elf64_Ehdr *a)
{
          int i;
          printf("_ident[]: ");
          for(i=0;i<16;i++)
          {
                  printf("%x ",a->_ident[i]);
          }
          printf("\n");
          printf("e_type = %d\n",a->e_type);
          printf("e_machine = %d\n",a->e_machine);
          printf("e_version = %d\n", a->e_version);
          printf("e_entry = %x\n",a->e_entry);
          printf("e_phoff = %d\n",a->e_phoff);
          printf("e_shoff = %d\n",a->e_shoff);
          printf("e_flags = %x\n",a->e_flags);
          printf("e_ehsize = %d\n",a->e_ehsize);
          printf("e_phentsize = %d\n",a->e_phentsize);
          printf("e_phnum = %d\n",a->e_phnum);
          printf("e_shentsize = %d\n",a->e_shentsize);
          printf("e_shnum = %d\n",a->e_shnum);
          printf("e_shstrndx = %d\n",a->e_shstrndx);

}

void print_section_info(Elf64_Shdr *s)
{
        printf("%x\t ",s->sh_name);
        printf("%x\t ",s->sh_type);
        printf("%x\t ",s->sh_flags);
        printf("%x\t ",s->sh_addr);
        printf("%x\t ",s->sh_offset);
        printf("%x\t ",s->sh_size);
        printf("%x\t ",s->sh_link);
        printf("%x\t ",s->sh_info);
        printf("%x\t ",s->sh_addralign);
        printf("%x",s->sh_entsize);
}



