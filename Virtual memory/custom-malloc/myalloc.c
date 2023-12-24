#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>

void * heapstart = NULL;
void * heapend;


struct header { // TODO should be single int, with low order bit for freeness
    unsigned int size;
    int free; 
};

void * myalloc(int size) {
    struct header* h;
    if (heapstart == NULL) {
        heapstart = mmap(NULL, 1 << 20, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | 
MAP_PRIVATE, -1, 0);
        heapend = heapstart;
    }
    void* p = heapstart;
    while (p < heapend) {
        h = (struct header*) p;
        if (h->free && h->size >= size) {
            h->free = 0;
            return (void*)(h+1);
        }
        p += sizeof(struct header) + h->size;
    }

    h = (struct header*) heapend;
    h->size = size;
    h->free = 0;
    heapend = (void*)(h+1) + size;
    return (void*)(h+1);
}

void myfree(void* p) {
    // TODO validate p
    struct header *h = (struct header*)p - 1;
    h->free = 1;
}


int main() {
    int * p = (int*) myalloc(4);
    char * q =(char*) myalloc(1000);
    *p = 42;
    *q = 'A';
    assert(*p == 42);
    assert(*q == 'A');
    myfree(p);
    int * r = (int*) myalloc(4);
    *r = 15;
    assert(*r == 15);
    assert(p == r);
    printf("%p, %p, %p\n", p, q, r);
}