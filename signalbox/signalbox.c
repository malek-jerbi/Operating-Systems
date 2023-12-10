#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t resized = 0;

void handle(int sig) {
    if (sig == SIGINT) {        
        printf("Caught SIGINT. the PID is %d, you have to kill it manually yourself using kill <process number>\n", getpid());
        //exit(0);
    }
}

void handle2(int sig) {
        resized = 1;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, handle);
    signal(SIGWINCH, handle2);
    while(1) {
        if (resized) {
            printf("Window resized\n");
            resized = 0;
        }
    }
}