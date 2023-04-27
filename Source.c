
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

long checkLenght(FILE *f);
int associateName(char s[]);
void checkRights(FILE *f, struct stat var);

void forFile(char* d, struct stat var){
    char s[50];
    struct timespec access_time = var.st_atim;

    FILE *f=fopen(d,"r");
                    if(f==NULL){
                        perror("File couldn't be opened\n");
                    }
            printf("\nRegular file options:\nname (-n),\nsize (-d),\nhard link count (-h),\ntime of last modification (-m),\naccess rights (-a)\n");
            scanf("%s",s);
            int j = associateName(s);
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
                case 5:
                    printf("The file %s was last accessed at: %ld seconds\n",
                        d, access_time.tv_sec);
                    break;
                default:
                    perror("Operation not correct!\n");

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

    /*if(S_ISREG(var.st_mode)){
        printf("%s is a regular file\n", argv[1]);
    } else if(S_ISDIR(var.st_mode)){
        printf("%s is a directory\n",argv[1]);
    } else if(S_ISLNK(var.st_mode)){
        printf("%s is a sybmolic link\n",argv[1]);
    }*/
    
\
    for(int i=1;i<argc;i++){
        if(S_ISREG(var.st_mode)){
            forFile(argv[i], var);
        
    }
    return 0;
    }
}