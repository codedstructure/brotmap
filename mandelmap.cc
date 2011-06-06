#include <stdio.h>
#include <math.h> // for NAN
#include "mandelmap.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int mpoint(pinfo* p);

int inside_points = 0;

// these are the dimensions of the fractal we are plotting.
const FLOAT MIN_X = -2.0;
const FLOAT MAX_X = 1.0;
const FLOAT MIN_Y = -1.5;
const FLOAT MAX_Y = 1.5;
const FLOAT BAILOUT = 4.0;

inline void mpoint(FLOAT r,
                   FLOAT i,
                   pinfo* p,
                   int old_max_iter=0)
{
    if (isnan(p->x)) {
        return;
    }
    FLOAT x2, y2;
    FLOAT x = p->x;
    FLOAT y = p->y;
    int k = MAX_ITER;

    // this is the core 'inner loop'.
    // It should be fast, which is why it doesn't
    // have lots of whitespace in it ;)
    //
    // of course this is massively parallelizable
    do {
        x2 = x*x;
        y2 = y*y;
        if ((x2+y2) >= BAILOUT) break;
        y = 2.0*x*y+r;
        x = x2-y2+i;
    } while (--k);

    if (!k)
    {
        // we ran out of iterations. Up to MAX_ITER,
        // this point seems to be part of the mset.
        // Store current values of x and y so we can
        // carry on later.
        p->x = x;
        p->y = y;
        inside_points ++;
    }
    else
    {
        // this is definitely not part of the mset
        // (rounding errors excepted). We want to store
        // that fact, together with an iteration count
        // and the magnitude of the calculation at that
        // point (so we can do smooth renderings)
        p->x = NAN;
        p->itercount = (old_max_iter + (MAX_ITER - k));
    }
}

int main()
{
    // for multiples of 2^-n (where n is < the number of mantissa digits)
    // floating point can exactly represent the values. Work out the
    // such a value by repeated dividing by two.
    FLOAT step = 1.0;
    unsigned long long invstep = 1;
    for (int i=0; i<BINARY_DIGITS; i++)
    {
        step /= 2;
        invstep *= 2;
    }
    const FLOAT plot_area = (MAX_Y-MIN_Y)*(MAX_X-MIN_X);  // hopefully an integer.
    const unsigned long long total_points = plot_area * invstep * invstep;


    // we will write the data to a file mandel_{BD}.dat where BD
    // is the 'n' in the step size 2^-n
    // a 'single precision' full map (23 bits mantissa) would take
    // up nearly nearly 10 petabytes (2^50) of storage. Much better
    // use than most of the data out there, me thinks.
    const unsigned long long mapsize = total_points*sizeof(pinfo) + HEADER_LEN;
    char fname[21];
    snprintf(fname, 20, "mandel_%d.dat", BINARY_DIGITS);
    int fd = open(fname, O_RDWR|O_CREAT, 0644);
    ftruncate(fd, mapsize);

    // we will use mmap to write to the data file. It makes things wonderful.
    void* mapping;
    if (NULL == (mapping = mmap(0, mapsize,
            PROT_READ|PROT_WRITE,
            MAP_FILE|MAP_SHARED|MAP_NOCACHE,
            fd, 0)))
    {
        return 1;
    }
    close(fd);  // we no longer need the FD when we have the mapping

    brotfile_header* fheader = (brotfile_header*)(mapping);
    pinfo* fptr = (pinfo*)((char*)mapping+HEADER_LEN);
    void* sourceBuffer = fptr;  // for unmmaping later

    printf("mapped file %d at %p %llu bytes\n", fd, fptr, mapsize);

    unsigned long long z = 0;
    for (FLOAT y=MIN_Y; y<MAX_Y; y+=step)
    {
        for (FLOAT x=MIN_X; x<MAX_X; x+=step)
        {
            pinfo* p=fptr++;
            mpoint(y, x, p, fheader->max_iter);
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
    printf("%llu %d (%.6f)\n", total_points, inside_points, ((plot_area*inside_points)/float(total_points)));
}
