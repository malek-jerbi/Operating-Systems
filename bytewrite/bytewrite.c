#include <stdio.h>
#include <sys/stat.h>

#define ONE_MEG 1 << 20

int main(int argc, char *argv[]) {
    FILE *fptr;
    fptr = fopen("filename.txt", "w");
    int prior_blocks = -1;
    struct stat stats;
    for (int i = 0; i < ONE_MEG ; i++) 
        fputc('a', fptr);
        stat("/filename.txt", &stats);
        if (stats.st_blocks != prior_blocks){
            printf("Size: %lld, blocks: %lld", stats.st_size, stats.st_blocks);
            prior_blocks = stats.st_blocks;
        }
    fclose(fptr);

}
