#define _DEFAULT_SOURCE
#define st_mtime st_mtim.tv_sec
#define MAX_BUFFER_SIZE 1024
#define MAX_COMMAND_SIZE 1024

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
#include <stdbool.h>
#include <sys/wait.h>

struct errAndWar{
    int errors, warnings;
};


int associateName(char s[]);
void checkRights(struct stat var);
void createSymLink(char *d, int fd[]);
bool checkCFile(char *d);
void checkErrors(char *d, int fd[]);
void countLines(FILE *f, char *d);
int computeScore(struct errAndWar Data);

void forFile(char* d, struct stat var){
    char s[50];
    int fd[2];
    pid_t pid,pid2;
    int status;


    FILE *f=fopen(d,"r");
    if(f==NULL){
        perror("File couldn't be opened\n");
    }
    bool check = false, k = true;
    while(!check){
        k = true;
        printf("\nRegular file options:\n name (-n),\n size (-d),\n hard link count (-h),\n time of last modification (-m),\n access rights (-a)\n create symbolic link (-l)\n\n");
        fgets(s,50,stdin);
        if(s[0]=='-'){
            for(int i=1;i<strlen(s)-1;i++){
                if(strchr("ndhaml",s[i])==NULL)
                    k = false;
            if(k)
                check = true;
            }
        }
        if(!check || s[0]!='-'){
            printf("\nCommand not correct! Enter again:\n");
        }
    }
    
    for(int i=1;i<strlen(s)-1;i++){
        pid = fork();
        if(pid<0){
            fprintf(stderr, "Failed to fork a new process\n.");
            exit(0);

        } else if(pid==0){

            switch(s[i]){
                case 'n':
                    printf("File name: %s\n",d);
                    break;
                case 'd':
                    printf("The file %s is %ld bytes long\n", d, (long)var.st_size);
                    break;
                case 'h':
                    printf("The file %s has %ld hard links\n", d, (long)var.st_nlink);
                    break;
                case 'a':
                    checkRights(var);
                    break;
                case 'm':
                    printf("Last modified time: %s\n", ctime(&var.st_mtime));
                    break;
                case 'l':
                    createSymLink(d, fd);
                    break;
                default:
                    perror("Operation not correct!\n");

            }
            exit(0);
            
        }else{
            if(i==strlen(s)-2){
                    
                for (int i = 0; i < strlen(s)-2; i++) {
                    pid_t child_pid = waitpid(-1, &status, 0);

                    if (WIFEXITED(status)) {
                        int exit_code = WEXITSTATUS(status);
                        printf("\nThe process with PID %d has ended with the exit code %d\n", child_pid, exit_code);
                    } else {
                        printf("\nThe process with PID %d has ended abnormally\n", child_pid);
                    }
                }
                int fd[2];
                if(pipe(fd) == -1){
                    perror("Couldn't create pipe\n");
                    exit(0);
                }
                pid2 = fork();

                if(pid2==0){
                    if(checkCFile(d)){
                        checkErrors(d,fd);
                    }
                    else{
                        //printf("\nThe no. of lines in the %s file: %d.\n", d, countLines(f));
                        fclose(f);
                        countLines(f,d);
                            
                    }
                    exit(0);
                } else{
                        
                    if(checkCFile(d)){
                        close(fd[1]);
                        struct errAndWar Data;
                        int len = read(fd[0], &Data, sizeof(Data));
                        if(len!=sizeof(Data)){
                            perror("Couldn't read from pipe\n");
                            exit(0);
                        }
                        else{

                            //printf("Err: %d and war: %d\n", Data.errors, Data.warnings);
                            int score = computeScore(Data);
                            FILE *ff = fopen("grades.txt","a+");
                            if(!ff){
                                perror("Couldn't open the grades.txt file\n");
                                exit(0);
                            }
                            else{
                                printf("Score was successfully written in the grades.txt file\n");
                            }
                            

                            fprintf(ff,"File %s has the score: %d\n",d,score);


                        }
                        close(fd[0]);
                        
                    }
                    wait(&status);
                    if (WIFEXITED(status)) {
                        int exit_code = WEXITSTATUS(status);
                        printf("\nThe process with PID %d has ended with the exit code %d\n", pid2, exit_code);
                    } else {
                        printf("\nThe process with PID %d has ended abnormally\n", pid2);
                    }
                        
                }
            }
                
        }
    }
    fclose(f); 
}



   

void checkRights(struct stat var){
    printf("Owner:\n\nRead: %s\nWrite: %s\nExec: %s\n\n", 
    ((var.st_mode & S_IRUSR) ? "yes" : "no"),
    ((var.st_mode & S_IWUSR) ? "yes" : "no"),
    ((var.st_mode & S_IXUSR) ? "yes" : "no"));

    printf("Group:\n\nRead: %s\nWrite: %s\nExec: %s\n\n", 
    ((var.st_mode & S_IRGRP) ? "yes" : "no"),
    ((var.st_mode & S_IWGRP) ? "yes" : "no"),
    ((var.st_mode & S_IXGRP) ? "yes" : "no"));

    printf("Others:\n\nRead: %s\nWrite: %s\nExec: %s\n\n", 
    ((var.st_mode & S_IROTH) ? "yes" : "no"),
    ((var.st_mode & S_IWOTH) ? "yes" : "no"),
    ((var.st_mode & S_IXOTH) ? "yes" : "no"));
}

void createSymLink(char *d, int fd[]){

    
    char name[100];
    printf("Input name of the symbolic link:\n");
    fgets(name,50,stdin);
    name[strlen(name)-1] = '\0';

    if(!symlink(d, name)){
        printf("Symbolic link created successfully\n");
    }
    else{
        perror("Couldn't create symbolic link\n");
        exit(0);
    }
}

bool checkCFile(char *d){
    char *extension = strrchr(d,'.');
    if(extension && !strcmp(extension,".c")){
            return true;
        }
    return false;
}


void checkErrors(char *d, int fd[]){
    
    char buffer[MAX_BUFFER_SIZE];
    char command[MAX_COMMAND_SIZE];
    sprintf(command,"./script.sh %s", d);

    FILE* pipe= popen(command,"r");
    if(!pipe){
        perror("Couldn't open the pipe for script.sh");
        exit(0);
    }
    
    printf("\n");


    int ok = 1;
    int warnings = 0, errors = 0;
    while(fgets(buffer, MAX_BUFFER_SIZE, pipe)){
        for(int i=0;i<strlen(buffer);i++){
            if(isdigit(buffer[i])){
                if(ok == 1){
                    warnings = warnings * 10 + buffer[i]-'0';
                }
                else{
                    errors = errors * 10 + buffer[i]-'0';
                }
            }
        }
        ok = 0;
    }

    struct errAndWar Data;
    Data.warnings = warnings;
    Data.errors = errors;

    close(fd[0]);
    write(fd[1],&Data,sizeof(Data));
    close(fd[1]);

    pclose(pipe);
}

void countLines(FILE *f, char *d){

    
    char command[MAX_COMMAND_SIZE];
    snprintf(command,MAX_COMMAND_SIZE,"wc -l %s",d);
    
    if((f=popen(command,"r"))==NULL){
        perror("Couldn't open file\n");
        exit(0);
    }
    int count;
    fscanf(f,"%d",&count);
    
    printf("The file %s contains %d lines\n",d,count);
    pclose(f);
}

int computeScore(struct errAndWar Data){
    int score;
    if(!Data.errors && !Data.warnings)
        score = 10;
        else if(Data.errors>0)
            score = 1;
            else if(!Data.errors && Data.warnings>10)
                score = 2;
                else if(!Data.errors && Data.warnings<=10)
                    score = 2+8*(10-Data.warnings)/10;
    return score;
}


/*////////////////////////////////////////////////////////////////////////////////////*/


int checkCFiles(DIR *dir);
void createDirFile(char *d);

void forDirectories(char *d, struct stat var){

    char s[50];
    int status;

    DIR *dir;
    dir = opendir(d);
    if(dir==NULL){
        perror("Couldn't open\n");
        exit(0);
    }
    bool check = false, k = true;
    while(!check){
        k = true;
        printf("\nDirectory options:\n name (-n),\n size (-d),\n access rights (-a),\n total number of files with the .c extension (-c)\n\n");
        fgets(s,50,stdin);

        if(s[0]=='-'){
            for(int i=1;i<strlen(s)-1;i++){
                if(strchr("ndac",s[i])==NULL)
                    k = false;
            if(k)
                check = true;
            }
        }
        if(!check || s[0]!='-'){
            printf("\nCommand not correct! Enter again:\n");
        }
    }

    printf("\nDirectory options:\n name (-n),\n size (-d),\n access rights (-a),\n total number of files with the .c extension (-c)\n\n");
    fgets(s,50,stdin);

    pid_t pid2,pid;

    for(int i=1;i<strlen(s)-1;i++){
    
        pid = fork();
        if(pid<0){
            fprintf(stderr, "Failed to fork a new process\n.");
            exit(0);
        } else if(pid==0){
            switch(s[i]){
            case 'n':
                printf("Directory name: %s\n",d);
                break;
            case 'd':
                printf("The directory %s is %ld bytes long\n", d, (long)var.st_size);
                break;
            case 'a':
                checkRights(var);
                break;
            case 'c':
                printf("The directory contains %d files with the extension '.c'\n", checkCFiles(dir));
                break;
            default:
                perror("Operation not correct!\n");
            }
             exit(0);
        }else{
            if(i==strlen(s)-2){
                for (int i = 0; i < strlen(s)-1; i++) {
                    pid_t child_pid = waitpid(-1, &status, 0);

                    if (WIFEXITED(status)) {
                        int exit_code = WEXITSTATUS(status);
                        printf("\nThe process with PID %d has ended with the exit code %d\n", child_pid, exit_code);
                    } else {
                        printf("\nThe process with PID %d has ended abnormally\n", child_pid);
                    }
                }
            }

            if(i == strlen(s)-2){

                pid2 = fork();
                if(pid2<0){
                    perror("Couldn't fork second process\n");
                    exit(0);
                }
                else if(pid2 == 0){
                    createDirFile(d);
                    exit(0);
                }else{
                    pid_t child_pid = waitpid(-1, &status, 0);

                    if (WIFEXITED(status)) {
                        int exit_code = WEXITSTATUS(status);
                        printf("\nThe process with PID %d has ended with the exit code %d\n", child_pid, exit_code);
                    } else {
                        printf("\nThe process with PID %d has ended abnormally\n", child_pid);
                    }
                }
            }
        }
    }
    closedir(dir);
}  

void createDirFile(char *d){
    char command[MAX_COMMAND_SIZE];
    snprintf(command,MAX_COMMAND_SIZE,"touch %s_file.txt",d);
    system(command);

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
void symMenu(char s, struct stat var, char *link, int *x);
void changeAcessRights(char *linkName);

void forSymLink(char *link, struct stat var){

    char s[50];
    int status;
    int x = 1;

    bool check = false, k = true;
    while(!check){
        k = true;
        printf("Options for symbolic link:\n name (-n),\n delete symbolic link (-l),\n size of symbolic link (-d),\n size of target file (-t),\n access rights (-a)\n\n");
        fgets(s,50,stdin);

        if(s[0]=='-'){
            for(int i=1;i<strlen(s)-1;i++){
                if(strchr("nldta",s[i])==NULL)
                    k = false;
            if(k)
                check = true;
            }
        }
        if(!check || s[0]!='-'){
            printf("\nCommand not correct! Enter again:\n");
        }
    }

    
    pid_t pid, pid2;
    printf("\n");

    for(int i=1;i<strlen(s)-1 && x == 1;i++){

        pid = fork();
        if(pid<0){
            fprintf(stderr, "Failed to fork a new process\n.");
            exit(0);
        } else if(pid==0 && x == 1){
            symMenu(s[i], var, link, &x);
        }else{

            if(i==strlen(s)-2){
                for (int i = 1; i < strlen(s)-1; i++) {
                    pid_t child_pid = waitpid(-1, &status, 0);

                    if (WIFEXITED(status)) {
                        int exit_code = WEXITSTATUS(status);
                        printf("\nThe process with PID %d has ended with the exit code %d\n", child_pid, exit_code);
                    } else {
                        printf("\nThe process with PID %d has ended abnormally\n", child_pid);
                    }
                }
            }

            if(x==-1)
                break;
                
            if(i == strlen(s)-2){
                    
                pid2 = fork();
                if(pid2<0){
                    perror("Couldn't fork second process\n");
                    exit(0);
                }
                else if(pid2 == 0){
                    changeAcessRights(link);
                }
                else{
                    pid_t child_pid = waitpid(-1, &status, 0);

                    if (WIFEXITED(status)) {
                        int exit_code = WEXITSTATUS(status);
                        printf("\nThe process with PID %d has ended with the exit code %d\n", child_pid, exit_code);
                    } else {
                        printf("\nThe process with PID %d has ended abnormally\n", child_pid);
                    }
                }
                    
            }
            pid_t child_pid = waitpid(-1, &status, 0);
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                printf("\nThe process with PID %d has ended with the exit code %d\n", child_pid, exit_code);
            } else {
                printf("\nThe process with PID %d has ended abnormally\n", child_pid);
                
            }
        }
    }
}


void symMenu(char s, struct stat var, char *link, int *x){
    switch(s){
        case 'n':
            printf("Symbolic link name: %s\n",link);
            break;
        case 'd':
            printf("The symbolic link %s is %ld bytes long\n", link, (long)var.st_size);
            break;
        case 'a':
            checkRights(var);
            break;
        case 'l':
            if(unlink(link) == -1){
                perror("Couldn't unlink\n");
                exit(0);
            }
            else{
                printf("Unlinked successfully.\n");
                *x = -1;
                return;
            }
            break;
        case 't':
            getTargetStatus(link,var);
            break;
        default:
        perror("Operation not correct!\n");
    }
    //*x = 1;
}

void getTargetStatus(char *linkName, struct stat var){
    struct stat target;
    if(stat(linkName, &target) == -1){
        perror("Target file not found\n");
        exit(0);
    }
    printf("Size of %s's target is: %ld bytes\n", linkName, target.st_size);
}

void changeAcessRights(char *linkName){

    char command[MAX_COMMAND_SIZE];
    sprintf(command,"./scriptSymLnk.sh %s", linkName);

    FILE* pipe= popen(command,"r");
    if(!pipe){
        perror("Couldn't open the pipe for script.sh");
        exit(0);
    }
    else{
        printf("Rights changed successfully\n");
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
            printf("\nFor the regular file: %s\n", argv[i]);
            forFile(argv[i], var);
        }
        if(S_ISDIR(var.st_mode)){
            printf("\nFor the directory: %s\n", argv[i]);
            forDirectories(argv[i], var);
        }
        if(S_ISLNK(var.st_mode)){
            printf("\nFor the symbolic link: %s\n", argv[i]);
            forSymLink(argv[i],var);
        }
    }
    return 0;
}