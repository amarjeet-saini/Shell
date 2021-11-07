/*
* osh shell - A UNIX Shell implementation 
* For documentation visit - 
* Author - Amarjeet Saini
* Created on - 20/10/21
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define PATH_MAX 80

// path to search for any cmnd
char Path1[] = "/bin/";
char Path2[] = "/usr/bin/";

//node to store command
struct histNode {
    char data[20];
    struct histNode* next;
};

//for  making queue 
struct histNode* first = NULL; // most recent cmmd which was i/p 
struct histNode* last = NULL; // last cmmd which was i/p
static int count = 0;

struct histNode* createhNode (char s[]) {
    struct histNode* temp = (struct histNode*) malloc(sizeof(struct histNode));  
    strcpy(temp->data,s); 
    temp->next = NULL;
    return temp;
}

void deleteEnd(void) {
    /*
    * delete last node from the queue which is last cmmd user entered  
    * argument: void
    * return: void
    */
    struct histNode* p = first;
    while(p->next != last) {
        p = p->next;
    }
    p->next = NULL;
    free(last);
    last = p;
}

void insertAtBegin(char s[]) {
    /*
    * insert most recent command @ front of queue 
    * function call: insertAtBegin("ls");
    * argument: char * 
    * return: void
    */
    s[strcspn(s, "\n")] = 0; // removes /n from string for display purpose
    static const int N = 10; // can only store atmost 10 commands 
    if (count >= N) {
        //delete last node
        deleteEnd();
    }
    count++;
    if  (first== NULL && last == NULL) {
        first = createhNode(s);
        last = first;
    } 
    else {
        struct histNode* p = createhNode(s);
        p->next = first;
        first = p;
    }
}


void displayHistory(){
    /*
    * display all commands in queue by iterate through it
    *   return : void
    */
    struct histNode* p = first;
    int temp = 1;
    while (p) {
        printf("%d  %s\n", temp, p->data);
        temp++;
        p = p->next;
    }
}

void execute (char s[], char *args[]) {
    /*  replace address space of child process with user input address space and executes it
    *   char *args[] = {"ls","-al",NULL};
    *   function call: execute(Path1,args);
    *   argument: char arr[], arr of char pointer
    *   return : void
    */
    if(execv(s, args) == -1) {
        fprintf(stderr,"the command does not exist\n");
        exit(1);
    }
}

int main (int argc, char *argv[]) {
    int run = 1;

    if (argc > 2) {
        fprintf(stderr,"error : shell invoked with more argument\n");
        exit(1);     
    }

    // interactive  mode of shell
    while (run) {
        printf("osh> ");
        fflush(stdout);
        char *buffer, bufferT[PATH_MAX];
        size_t size = 0;

        //get one user command from shell as input
        if (getline(&buffer, &size, stdin) == -1) {
            // error reading
            fprintf(stderr,"error reading input");
            exit(1);
        }

        strcpy(bufferT,buffer);
        insertAtBegin(bufferT);

        // user enter "exit\n" terminate main process
        if (strcmp(buffer,"exit\n") == 0) {
            exit(0);
        }

        // cmmd pwd (print current working directory)
        else if((strcmp(buffer,"pwd\n") == 0)) {
            char cwd [PATH_MAX];
            if (getcwd(cwd,sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                fprintf(stderr,"getcwd() error\n");
                return -1;
            }
        }
        // cmmd history (display previous cmmd including current one)
        else if (!strcmp(buffer, "history\n")) {
            displayHistory();
        }

        else {
            int i = 0;    
            char *end, *r, *tok;
            char *args[PATH_MAX/2];
            r = end = strdup(buffer);
            //printf("%s %s\n",r, end);
            assert(end != NULL);
            // to handle any amnt of space and /n issue i.e ls -l\n
            while ((tok = strsep(&end, " ")) != NULL) {
                if (strcmp(tok,"") != 0) {
                    args[i] = tok;
                    // remove /n from cmnd prompt eg: ls\n 
                    //https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
                    args[i][strcspn(args[i], "\n")] = 0;
                    i++;
                }
            }
            args[i] = NULL;
            if (!strcmp(args[0], "cd")) {
                // Don't fetch the current dir
                chdir(args[1]);
                continue;
                }
            int pid = fork();
            if (pid < 0) {
                fprintf(stderr,"fork() failed\n");
                exit(1);
            }
            // child process
            else if (pid == 0) {

                // implement > redirection
                int redirect = 0;
                int j = 0;
                for(; j < i; j++) {
                    if(strcmp(args[j],">") == 0) { redirect = 1; break;} 
                }

                char *argsr[10]; 
                
                if (redirect) {
                    int k=0;
                    for(; k < j; k++) {
                        argsr[k] = args[k];
                    }
                    argsr[j] = NULL;
                    int fd = open(args[j+1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                    dup2(fd, 1);   // make stdout go to file
                    dup2(fd, 2);    // make stderr go to file - you may choose to not do this
                    close(fd);
                }
                
                if (access(strcat(Path1, args[0]), X_OK) != -1) { //success /bin
                    if (redirect) execute(Path1, argsr);
                    else execute(Path1, args);
                } else if (access(strcat(Path2, args[0]), X_OK) != -1) { // success /usr/bin
                    if (redirect) execute(Path2, argsr);
                    else  execute(Path2, args);
                } else {
                    fprintf(stderr, "error file not found\n");
                    exit(1);
                }
                free(r);
            }
            // main process
            else {
                wait(NULL); // wait for child process to complete
            }
        }
    }
    return 0;
}