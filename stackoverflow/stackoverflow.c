#include <stdio.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/resource.h>

void recursive(int depth, long int bottom) {
    if (depth % 10000 == 0)
        printf("pid: %d depth: %d %ld (%p)\n", getpid(), depth,  bottom - (long) &depth, &depth);
    recursive(depth+1, bottom);
}

start() {
    int depth = 0;
    recursive(depth, (long) &depth);
}

int main(int argc, char *argv[]) {
    struct rlimit r;
    getrlimit(RLIMIT_STACK, &r);
    struct rlimit rnew;
    rnew.rlim_max = r.rlim_max;
    rnew.rlim_cur = r.rlim_max;
    setrlimit(RLIMIT_STACK, &rnew);
    printf("%d %d", rnew.rlim_cur, rnew.rlim_max);
    start();
}