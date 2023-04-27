#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char** argv){
    if(argc<2){
        printf("There aren't any arguments!\n");
        exit(0);
    }

    struct stat var;
    if(stat(argv[1], &var) == -1){
        perror("stat");
        exit(0);
    }

    if(S_ISREG(var.st_mode)){
        printf("%s is a regular file\n", argv[1]);
    } else if(S_ISDIR(var.st_mode)){
        printf("%s is a directory\n",argv[1]);
    } else if(S_ISLNK(var.st_mode)){
        printf("%s is a sybmolic link\n",argv[1]);
    }
    return 0;
}