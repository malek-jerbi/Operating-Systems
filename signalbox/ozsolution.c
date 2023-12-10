#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int sig) {
        printf("Window resized\n");
}

int main(int argc, char* argv[]) {
    struct sigaction sig;
    sig.sa_handler = handler;
    sigaction(SIGWINCH, &sig, NULL);
    for(;;);
        
}