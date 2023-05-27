#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./setup [argument]\nHelp for more informations\n");
        return 1;
    }
    else if (strcmp(argv[1], "setupKrnl") == 0)
    {
        system("git clone https://github.com/Kokolor/KernelTemplate.git");
        system("mv KernelTemplate/ krnlSource/");
        system("cp -r lunarForge krnlSource/kernel/src/");
    }
    else if (strcmp(argv[1], "help") == 0)
    {
        printf("setupKrnl - Setup kernel\n");
    }
    else
    {
        printf("Unknown command!\n");
        return 1;
    }

    return 0;
}
