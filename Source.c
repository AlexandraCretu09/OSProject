#define _DEFAULT_SOURCE
#define st_mtime st_mtim.tv_sec

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

int associateName(char s[]);
void checkRights(struct stat var);
void createSymLink(char *d, struct stat var);

void forFile(char* d, struct stat var){
    char s[50];
    pid_t pid;

    FILE *f=fopen(d,"r");
    if(f==NULL){
        perror("File couldn't be opened\n");
    }
    

    printf("\nRegular file options:\n name (-n),\n size (-d),\n hard link count (-h),\n time of last modification (-m),\n access rights (-a)\n create symbolic link (-l)\n\n");
    fgets(s,50,stdin);
    char *p = malloc(sizeof(p));
    
    p = strtok(s," ");
    while(p!=NULL){
        int j = associateName(p);

        pid = fork();
        if(pid<0){
            fprintf(stderr, "Failed to fork a new process\n.");
            exit(0);
        }
        else if(pid==0){
            switch(j){
                case 1:
                    printf("File name: %s\n",d);
                    break;
                case 2:
                    printf("The file %s is %ld bytes long\n", d, (long)var.st_size);
                    break;
                case 3:
                    printf("The file %s has %ld hard links\n", d, (long)var.st_nlink);
                    break;
                case 4:
                    checkRights(var);
                    break;
                case 5:
                    printf("Last modified time: %s", ctime(&var.st_mtime));
                    break;
                case 7:
                    createSymLink(d, var);
                    break;
                default:
                    perror("Operation not correct!\n");

            }
            exit(0);
        }
        p=strtok(NULL," ");
    }
    fclose(f);      
}

int associateName(char *s){
    int j;
    if(strncmp("-n",s,2)==0)
            j = 1;
            else if(strncmp("-d",s,2)==0)
                j = 2;
                else if(strncmp("-h",s,2)==0)
                    j = 3;
                    else if(strncmp("-a",s,2)==0)
                        j = 4;
                        else if(strncmp("-m",s,2)==0)
                            j = 5;
                            else if(strncmp("-c",s,2)==0)
                                j = 6;
                                else if(strncmp("-l",s,2)==0)
                                    j = 7;
                                    else if(strncmp("-t",s,2)==0)
                                        j = 8;
                                        else
                                            j = -1;
    return j;
}



void checkRights(struct stat var){
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

void createSymLink(char *d, struct stat var){
    char name[50];    
    printf("Input name of the symbolic link:\n");
    fgets(name,50,stdin);
    
    if(!symlink(d, name)){
        printf("Symbolic link created successfully\n");
    }
    else{
        perror("Couldn't create symbolic link\n");
        exit(0);
    }
}


/*/////////////////////////////////////////////////////////////////////////*/


int checkCFiles(DIR *dir);

void forDirectories(char *d, struct stat var){

    char s[50];

    DIR *dir;
    dir = opendir(d);
    if(dir==NULL){
        perror("Couldn't open\n");
        exit(0);
    }

    printf("\nDirectory options:\n name (-n),\n size (-d),\n access rights (-a),\n total number of files with the .c extension (-c)\n\n");
    fgets(s,50,stdin);
    char *p;

    p = strtok(s," ");

    while(p!=NULL){

        int j = associateName(p);
    
        pid_t pid;
        pid = fork();
        if(pid<0){
            fprintf(stderr, "Failed to fork a new process\n.");
            exit(0);
        }
        else if(pid == 0){
            switch(j){
                case 1:
                    printf("Directory name: %s\n",d);
                    break;
                case 2:
                    printf("The directory %s is %ld bytes long\n", d, (long)var.st_size);
                    break;
                case 4:
                    checkRights(var);
                    break;
                case 6:
                    printf("The directory contains %d files with the extension '.c'\n", checkCFiles(dir));
                    break;
                case 8:

                    break;
                default:
                    perror("Operation not correct!\n");
            }
            exit(0);
        }
        p=strtok(NULL," ");
    }
    closedir(dir);
}


int checkCFiles(DIR *dir){
    int count = 0;
    struct dirent* entry;
    while((entry = readdir(dir)) != NULL){
        if(entry->d_type == DT_REG){
            char *extension = strrchr(entry->d_name,'.');
            if(extension && !strcmp(extension,".c")){
                count++;
            }
        }
    }
    return count;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getTargetStatus(char *linkName, struct stat var);

void forSymLink(char *link, struct stat var){

    char s[50];

    printf("Options for symbolic link:\n name (-n),\n delete symbolic link (-l),\n size of symbolic link (-d),\n size of target file (-t),\n access rights (-a)\n\n");
    fgets(s,50,stdin);
    char *p;

    p = strtok(s," ");

    while(p!=NULL){

        int j = associateName(p);
    
        pid_t pid;
        pid = fork();
        if(pid<0){
            fprintf(stderr, "Failed to fork a new process\n.");
            exit(0);
        }
        else if(pid == 0){
            switch(j){
                case 1:
                    printf("Symbolic link name: %s\n",link);
                    break;
                case 2:
                    printf("The symbolic link %s is %ld bytes long\n", link, (long)var.st_size);
                    break;
                case 4:
                    checkRights(var);
                    break;
                case 7:
                    if(unlink(link) == -1){
                        perror("Couldn't unlink\n");
                        exit(0);
                    }
                    else{
                        printf("Unlinked successfully.\n");
                    }
                    break;
                case 8:
                    getTargetStatus(link,var);
                    break;
                default:
                    perror("Operation not correct!\n");
            }
            exit(0);
        }
        p=strtok(NULL," ");
    }
}

void getTargetStatus(char *linkName, struct stat var){
    struct stat target;
    if(stat(linkName, &target) == -1){
        perror("Target file not found\n");
        exit(0);
    }
    printf("Size of %s's target is: %ld bytes\n", linkName, target.st_size);
}


int main(int argc, char** argv){

    if(argc<2){
        printf("There aren't any arguments!\n");
        exit(0);
    } 
    for(int i=1;i<argc;i++){

        struct stat var;
        if(lstat(argv[i], &var) < 0){
            perror("Couldn't get file status\n");
            exit(0);
        }
        
        if(S_ISREG(var.st_mode)){
            forFile(argv[i], var);
        }
        if(S_ISDIR(var.st_mode)){
            forDirectories(argv[i], var);
        }
        if(S_ISLNK(var.st_mode)){
            forSymLink(argv[i],var);
        }
    }
    return 0;
}