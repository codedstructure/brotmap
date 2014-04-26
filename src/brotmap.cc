// brotmap - Ben Bass 2010-2014 @codedstructure

#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include "brotmap.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#ifndef _POSIX_MAPPED_FILES
#error "no mapped file support"
#endif

#ifndef MAP_NOCACHE
#define MAP_NOCACHE 0
#endif

#ifndef ftruncate64
#define ftruncate64 ftruncate
#endif

#ifndef mmap64
#define mmap64 mmap
#endif

long inside_points = 0;

// these are the dimensions of the fractal we are plotting.
FLOAT MIN_X = -2.0;
FLOAT MAX_X = 1.0;
FLOAT MIN_Y = -1.5;
FLOAT MAX_Y = 1.5;

int BINARY_DIGITS = 10;


int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("Usage: %s DATA_FILENAMES BINARY_DIGITS\n", argv[0]);
        return 2;
    }
    BINARY_DIGITS = atoi(argv[2]);
    if (BINARY_DIGITS < 4 || BINARY_DIGITS > 24) {
        // 24 is a bit generous, but would be nice to see :-)
        printf("BINARY_DIGITS must be in range 4..24\n");
        return 2;
    }
    // for multiples of 2^-n (where n is < the number of mantissa digits)
    // floating point can exactly represent the values. Work out the
    // such a value by repeated dividing by two.
    //
    // TODO: this is all constant, so use a bit of TMP on it. Just for fun
    FLOAT step = 1.0;
    unsigned long long invstep = 1;
    for (int i=0; i<BINARY_DIGITS; i++)
    {
        step /= 2;
        invstep *= 2;
    }
    const FLOAT plot_area = (MAX_Y-MIN_Y)*(MAX_X-MIN_X);  // hopefully an integer.
    // following assumes we're drawing a square.
    const unsigned long long total_points = plot_area * invstep * invstep;

    // we will write the data to a file mandel_{BD}.dat where BD
    // is the 'n' in the step size 2^-n
    // a 'single precision' full map (23 bits mantissa) would take
    // up nearly 10 petabytes (2^50) of storage. Much better use
    // than most of the data out there, me thinks.
    const unsigned long long mapsize = total_points*sizeof(pinfo) + HEADER_LEN;
    int fd = open(argv[1], O_RDWR|O_CREAT, 0644);
    // TODO: read the header from the file, and if some magic constant
    // is absent, refuse to continue (in event it already exists)
    // Would also allow BINARY_DIGITS to be omitted if file already
    // exists
    if (!fd) {
        // 24 is a bit generous, but would be nice to see :-)
        printf("DATA_FILENAME could not be opened\n");
        return 1;
    }
    if (ftruncate64(fd, mapsize) != 0) {
        printf("ftruncate failed\n");
        return 1;
    }

    // we will use mmap to write to the data file. It makes things wonderful.
    void* mapping;
    if (NULL == (mapping = mmap64(0, mapsize,
            PROT_READ|PROT_WRITE,
            MAP_FILE|MAP_SHARED|MAP_NOCACHE,
            fd, 0)))
    {
        printf("mmap failed\n");
        return 1;
    }
    close(fd);  // we no longer need the FD when we have the mapping

    void* sourceBuffer = mapping;  // for unmmaping later
    brotfile_header* fheader = (brotfile_header*)(mapping);
    pinfo* fptr = (pinfo*)((char*)mapping+HEADER_LEN);

    printf("mapped file %d at %p %llu bytes\n", fd, fptr, mapsize);
    printf("Current MAX_ITER: %d\n", fheader->max_iter);

    worker_run(fptr, step, fheader->max_iter);

    fheader->max_iter += MAX_ITER;
    fheader->binary_digits = BINARY_DIGITS;

    if (sourceBuffer) {
        munmap(sourceBuffer, mapsize);
    }
    // print total pixels, 'inside' pixels, and area of mset based on
    // these figures.
    // Should be approx 1.50648
    // https://www.fractalus.com/kerry/articles/area/mandelbrot-area.html
    printf("%llu %lu (%.6f)\n", total_points, inside_points, ((plot_area*inside_points)/float(total_points)));
}
