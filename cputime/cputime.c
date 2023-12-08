#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <sched.h>
#include <sys/resource.h>
#define SLEEP_SEC 3
#define NUM_MULS 100000000
#define NUM_MALLOCS 100000
#define MALLOC_SIZE 1000
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


// TODO define this
struct profile_times {
  double real_start;
  double real_end;
  double cpu_start_time;
  double cpu_finish_time;
  double cpu_duration;
  int cpu;

  double user_start;
  double system_start;
};

// TODO populate the given struct with starting information
void profile_start(struct profile_times *t) {
  t->cpu = sched_getcpu();
  t->cpu_start_time = (double) clock();
  t->real_start = time(NULL);
  struct rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  t->user_start = ru.ru_utime.tv_sec * 1000000 + ru.ru_utime.tv_usec;
  t->system_start = ru.ru_stime.tv_sec * 1000000 + ru.ru_stime.tv_usec;
}

// TODO given starting information, compute and log differences to now
void profile_log(struct profile_times *t) {
  t->cpu_finish_time = (double) clock();
  t->cpu_duration = (t->cpu_finish_time - t->cpu_start_time) / CLOCKS_PER_SEC;
  t->real_end = time(NULL);
  const char* color;
  if (t->cpu == 0) {
    color = ANSI_COLOR_RED;
  } else if (t->cpu == 1) {
    color = ANSI_COLOR_GREEN;
  } 

  struct rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  double user_time =( ru.ru_utime.tv_sec * 1000000 + ru.ru_utime.tv_usec ) - t->user_start;
  double system_time =( ru.ru_stime.tv_sec * 1000000 + ru.ru_stime.tv_usec ) - t->system_start;
   printf("%s[pid %d, ", color, getpid());
  printf("cpu %d] ", t->cpu);
  printf("real: %.3fs ", t->real_end - t->real_start);
  printf("cputime: %.3fs ", t->cpu_duration);
  printf("user time: %.3fs ", user_time / 1000000);
  printf("system time: %.3fs%s\n ", system_time / 1000000, ANSI_COLOR_RESET);

}

int main(int argc, char *argv[]) {
  struct profile_times t;

  // TODO profile doing a bunch of floating point muls
  float x = 1.0;
  profile_start(&t);
  for (int i = 0; i < NUM_MULS; i++)
    x *= 1.1;
  profile_log(&t);

  // TODO profile doing a bunch of mallocs
  profile_start(&t);
  void *p;
  for (int i = 0; i < NUM_MALLOCS; i++)
    p = malloc(MALLOC_SIZE);
  profile_log(&t);

  // TODO profile sleeping
  profile_start(&t);
  sleep(SLEEP_SEC);
  profile_log(&t);
}


// TODO : color based on CPU