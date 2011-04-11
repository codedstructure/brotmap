#include <stdio.h>
#include <math.h> // for NAN
#include "mandelmap.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int mpoint(pinfo* p);

int inside = 0;

inline int mpoint(FLOAT r, FLOAT i, pinfo* p)
{
    if (p->x == NAN) {
        return p->itercount;
    }
    FLOAT x2, y2;
    FLOAT x = p->x;
    FLOAT i = x;
    FLOAT y = p->y;
    FLOAT r = y;
    int k = MAX_ITER;
    do {
        x2 = x*x;
        y2 = y*y;
        if ((x2+y2) >= 4.0) break;
        y = 2*x*y+r;
        x = x2-y2 + i;
    } while (--k);

    if (!k)
    {
        p->x = x;
        p->y = y;
        inside ++;
    }
    else
    {
        p->x = NAN;
        p->itercount = k;
    }
    return MAX_ITER - k;
}

int main()
{
    FLOAT step = 1.0;
    unsigned long long mapsize = 1.0;
    for (int i=0; i<BINARY_DIGITS; i++)
    {
        step /=2;
        mapsize *=2;
    }

    mapsize = 9*mapsize*mapsize*sizeof(pinfo);
    int fd = open("mandel.dat", O_RDWR|O_CREAT, 0666);
    lseek(fd, mapsize-1, SEEK_SET);
    write(fd, "\x00", 1);

    unsigned long long z =0;
    pinfo* fptr = (pinfo*)mmap(0, mapsize, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED|MAP_NOCACHE, fd, 0);
    if (!fptr) {
        return 1;
    }
    close(fd);
    void* sourceBuffer = fptr;

    printf("mapped file %d at %p %llu bytes\n", fd, fptr, mapsize);

    for (FLOAT y=-1.5; y<1.5; y+=step)
    {
        for (FLOAT x=-2.0; x<1.0; x+=step)
        {
            pinfo* p=fptr++;
            p->x = x;
            p->y = y;
            mpoint(p);
            z++;
        }
    }

    if (sourceBuffer)
        munmap(sourceBuffer, mapsize);
    printf("%llu %d\n", z, inside);
}
