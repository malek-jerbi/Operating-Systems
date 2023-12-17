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
#define MAX_CMDS 5
volatile pid_t childpid = 0;

void sigint_handler(int sig) {
    if (!childpid)
        return;
    if (kill(childpid, SIGINT) < 0)
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
        char *currToken;
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
        
        int argi = 0;
        int cmdi = 0;

        const char *commands[MAX_CMDS][MAX_ARGV];

        while (argi < MAX_ARGV && cmdi < MAX_CMDS) {
            commands[cmdi][argi++] = currToken;
            if (currToken == NULL) break;
            currToken = strtok_r(NULL, " \n", &saveptr);
            if (currToken != NULL && strcmp(currToken, "|") == 0) {
                commands[cmdi++][argi] = NULL;
                argi = 0;
                currToken = strtok_r(NULL, " \n", &saveptr);
            }
        }

        if (strcmp(commands[0][0], "quit") == 0)
            exit(0);


        // construct pipe
        int pipefd[2];
        int infd = 0;
        int childpids[cmdi + 1];
        for (int i = 0; i <= cmdi; i++) {
            if (i != cmdi) {
                if (pipe(pipefd) == -1) {
                    perror("pipe error");
                    exit(1);
                }
            }

            childpids[i] = fork();
            if (childpids[i] == 0) {
                if (i != cmdi) {
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                }
                dup2(infd, 0);
                if (execvp(commands[i][0], commands[i]) < 0) {
                    perror("execvp Error");
                    exit(1);
                }
                exit(0);
            }
            //parent
            infd = pipefd[0];
            if (i != cmdi) {
                close(pipefd[1]);
            }
                int status;
                waitpid(childpids[i], &status, 0); // Wait for child process to finish
            
        }
        
     }
}