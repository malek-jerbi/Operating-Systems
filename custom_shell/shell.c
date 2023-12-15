#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <setjmp.h>
#include <string.h> // Include for strtok_r
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#define MAX_CMD 150
#define MAX_ARGV 5

volatile pid_t childpid = 0;
volatile pid_t wcpid = 0;

void sigint_handler(int sig) {
    if (!childpid) return;
    if (kill(childpid, SIGINT) < 0)
        perror("Error sending SIGINT to child");
    if (!wcpid) return;
    if (kill(wcpid, SIGINT) < 0)
        perror("Error sending SIGINT to child");
    return;
}

int main(int argc, char* argv[]) {
    char s[MAX_CMD];
    char *saveptr;

    signal(SIGINT, sigint_handler);

    while(1) {    
        
        printf(">>");  
        
        fgets(s, MAX_CMD, stdin);
        char *currToken ;
        currToken = strtok_r(s, " \n", &saveptr);
        if ((currToken == NULL) && ferror(stdin)) {
            perror("fgets error");
            exit(1);
        }
        if (feof(stdin))
            exit(0);

        if (currToken == NULL) {
            continue;
        }
        
        int argc = 0;
        const char *argv[MAX_ARGV];

        while (currToken != NULL && argc < MAX_ARGV) {
            argv[argc++] = currToken;
            currToken = strtok_r(NULL, " \n", &saveptr);
        }

        if (strcmp(argv[0], "quit") == 0)
            exit(0);

        
        int pipefd[2];

        if (pipe(pipefd) == -1) {
            perror("pipe error");
            exit(1);
        }

        childpid = fork();
        if (childpid < 0) {
            perror("fork error\n");
            exit(1);
        }

        // TODO : remove these and get the right commands from the terminal
        char *argls[] = {"ls", NULL}; 

        char *argwc[] = {"wc", NULL}; 

        if (childpid == 0) {
            // child process
            close(pipefd[0]); // close read
            dup2(pipefd[1], STDOUT_FILENO); // redirect stdout to pipe write
            close(pipefd[1]); // close write
            if (execvp(argls[0], argls) < 0) {
                perror("cp1: execvp Error");
                exit(1);
            }
            
            exit(1);
        }
        else {
            close(pipefd[1]); // close write
            wcpid = fork();
            if (wcpid < 0) {
                perror("fork error\n");
                exit(1);
            }
            if (wcpid == 0) {
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
                //close(pipefd[0]);
                if (execvp(argwc[0], argwc) < 0) {
                    perror("wc: execvp Error");
                    exit(1);
                }
                printf("damn");
                exit(1);
            }
            
        }
        
        int status;
        waitpid(childpid, &status, 0);
        printf("finished");
        childpid = 0; 
    }
}