#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>

long checkLenght(FILE *f);
int associateName(char s[]);
void checkRights(FILE *f, struct stat var);

void forFile(char* d, struct stat var){
    char s[50];
    pid_t pid;

    FILE *f=fopen(d,"r");
    if(f==NULL){
        perror("File couldn't be opened\n");
    }
    

    printf("\nRegular file options:\nname (-n),\nsize (-d),\nhard link count (-h),\ntime of last modification (-m),\naccess rights (-a)\n");
    scanf("%s",s);
    int j = associateName(s);

    pid = fork();
    if(pid<0){
    fprintf(stderr, "Failed to fork a new process\n.");
            exit(0);
    }
    else if(pid==0){
        switch(j){
            case 1:
                printf("%s\n",d);
                break;
            case 2:
                printf("The file %s is %ld bytes long\n", d, (long)var.st_size);
                break;
            case 3:
                printf("The file %s has %ld hard links\n", d, (long)var.st_nlink);
                break;
            case 4:
                checkRights(f,var);
                break;
            default:
                perror("Operation not correct!\n");

        }
    exit(0);
    }
    fclose(f);      
}

int associateName(char s[]){
    int j;
    if(strcmp("-n",s)==0)
            j = 1;
            else if(strcmp("-d",s)==0)
                j = 2;
                else if(strcmp("-h",s)==0)
                    j = 3;
                    else if(strcmp("-a",s)==0)
                        j = 4;
                        else if(strcmp("-m",s)==0)
                            j = 5;
                            else if(strcmp("-c",s)==0)
                                j = 6;
    return j;
}



void checkRights(FILE *f, struct stat var){
    printf("Owner:\n    Read: %s\n    Write: %s\n    Exec: %s\n", 
        (var.st_mode & S_IRUSR) ? "yes" : "no",
        (var.st_mode & S_IWUSR) ? "yes" : "no",
        (var.st_mode & S_IXUSR) ? "yes" : "no");
    printf("Group: \n    Read: %s\n    Write: %s\n    Exec: %s\n", 
        (var.st_mode & S_IRGRP) ? "yes" : "no",
        (var.st_mode & S_IWGRP) ? "yes" : "no",
        (var.st_mode & S_IXGRP) ? "yes" : "no");
    printf("Others: \n    Read: %s\n    Write: %s\n    Exec: %s\n", 
        (var.st_mode & S_IROTH) ? "yes" : "no",
        (var.st_mode & S_IWOTH) ? "yes" : "no",
        (var.st_mode & S_IXOTH) ? "yes" : "no");
}

/*/////////////////////////////////////////////////////////////////////////*/

void checkRightsDir(DIR *f, struct stat var);

int checkCFiles(DIR *dir);

void forDirectories(char *d, struct stat var){
    char s[50];

    DIR *dir;
    dir = opendir(d);
    if(dir==NULL){
        perror("Couldn't open\n");
        exit(0);
    }
    printf("\nDirectory options:\nname (-n),\nsize (-d),\naccess rights (-a),\ntotal number of files with the .c extension (-c)\n");
    scanf("%s",s);
    int j = associateName(s);
    
    pid_t pid;
    pid = fork();
    if(pid<0){
        fprintf(stderr, "Failed to fork a new process\n.");
        exit(0);
    }
    else if(pid == 0){
        switch(j){
            case 1:
                printf("%s\n",d);
                break;
            case 2:
                printf("The directory %s is %ld bytes long\n", d, (long)var.st_size);
                break;
            case 4:
                checkRightsDir(dir,var);
                break;
            case 6:
                printf("The directory contains %d files with the extension '.c'\n", checkCFiles(dir));
                break;
            default:
                perror("Operation not correct!\n");
        }
    }
    closedir(dir);
}

void checkRightsDir(DIR *f, struct stat var){
    printf("Owner:\n    Read: %s\n    Write: %s\n    Exec: %s\n", 
        (var.st_mode & S_IRUSR) ? "yes" : "no",
        (var.st_mode & S_IWUSR) ? "yes" : "no",
        (var.st_mode & S_IXUSR) ? "yes" : "no");
    printf("Group: \n    Read: %s\n    Write: %s\n    Exec: %s\n", 
        (var.st_mode & S_IRGRP) ? "yes" : "no",
        (var.st_mode & S_IWGRP) ? "yes" : "no",
        (var.st_mode & S_IXGRP) ? "yes" : "no");
    printf("Others: \n    Read: %s\n    Write: %s\n    Exec: %s\n", 
        (var.st_mode & S_IROTH) ? "yes" : "no",
        (var.st_mode & S_IWOTH) ? "yes" : "no",
        (var.st_mode & S_IXOTH) ? "yes" : "no");
}

int checkCFiles(DIR *dir){
    int count = 0;

    struct dirent* entry;
    while(entry = readdir(dir)!=NULL){
        if(entry->d_type == DT_REG){
            char *file = entry->d_name;
            char *extension = strrchr(file,'.');
            if(extension && strcmp(extension,".c")==0)
                count++;
        }
    }
    return count;
}


int main(int argc, char** argv){
    if(argc<2){
        printf("There aren't any arguments!\n");
        exit(0);
    
    }
    
    /*
    if(S_ISREG(var.st_mode)){
        printf("%s is a regular file\n", argv[1]);
    } else if(S_ISDIR(var.st_mode)){
        printf("%s is a directory\n",argv[1]);
    } else if(S_ISLNK(var.st_mode)){
        printf("%s is a sybmolic link\n",argv[1]);
    }*/
    
    
    for(int i=1;i<argc;i++){

        struct stat var;
        if(stat(argv[i], &var) == -1){
            perror("stat");
            exit(0);
        }
        if(S_ISREG(var.st_mode)){
                forFile(argv[i], var);
        }
        if(S_ISDIR(var.st_mode)){
                forDirectories(argv[i], var);
        }
    }
    return 0;
}