#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <setjmp.h>

volatile uint64_t handled = 0;

int stopRecursive = 0;


sigjmp_buf jmp_env_seg;

void handle(int sig) {
  handled |= (1 << sig);
  printf("Caught %d: %s (%d total)\n", sig, sys_siglist[sig],
         __builtin_popcount(handled));
  if (sig == SIGINT) {
    exit(0);
  }

  if (sig== SIGSEGV) {
    siglongjmp(jmp_env_seg, 1);
  }
}

int main(int argc, char* argv[]) {
    
    // Register all valid signals
    for (int i = 0; i < NSIG; i++) {
        signal(i, handle);
    }

    int array[3] = {1, 2, 3};
    //int *p = 0;
    if (sigsetjmp(jmp_env_seg, 1) == 0){
      //*p = 5;
      printf("%d", array[99999]);
    }


    alarm(5);
    if (fork() == 0) {
      exit(0);
    }

    
    

    // spin
    for (;;)
      sleep(1);
}
