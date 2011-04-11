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

inline void mpoint(FLOAT r, FLOAT i, pinfo* p, int old_max_iter=0)
{
    if (isnan(p->x)) {
        return;
    }
    FLOAT x2, y2;
    FLOAT x = p->x;
    FLOAT y = p->y;
    int k = MAX_ITER;
    do {
        x2 = x*x;
        y2 = y*y;
        if ((x2+y2) >= 4.0) break;
        y = 2*x*y+r;
        x = x2-y2+i;
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
        p->itercount = (old_max_iter + (MAX_ITER - k));
    }
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

    mapsize = 9*mapsize*mapsize*sizeof(pinfo) + HEADER_LEN;
    char fname[21];
    snprintf(fname, 20, "mandel_%d.dat", BINARY_DIGITS);
    int fd = open(fname, O_RDWR|O_CREAT, 0664);
    ftruncate(fd, mapsize);

    unsigned long long z =0;
    void* mapping = mmap(0, mapsize, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED|MAP_NOCACHE, fd, 0);
    if (!mapping) {
        return 1;
    }
    close(fd);
    brotfile_header* fheader = (brotfile_header*)(mapping);
    pinfo* fptr = (pinfo*)((char*)mapping+HEADER_LEN);
    void* sourceBuffer = fptr;  // for unmmaping later

    printf("mapped file %d at %p %llu bytes\n", fd, fptr, mapsize);

    for (FLOAT y=-1.5; y<1.5; y+=step)
    {
        for (FLOAT x=-2.0; x<1.0; x+=step)
        {
            pinfo* p=fptr++;
            mpoint(y, x, p, fheader->max_iter);
            z++;
        }
    }

    fheader->max_iter += MAX_ITER;
    fheader->binary_size = BINARY_DIGITS;

    if (sourceBuffer) {
        munmap(sourceBuffer, mapsize);
    }
    // print total pixels, 'inside' pixels, and area of mset based on
    // these figures.
    // Should be approx 1.50648
    // https://www.fractalus.com/kerry/articles/area/mandelbrot-area.html
    printf("%llu %d (%.6f)\n", z, inside, ((9.0*inside)/float(z)));
}
