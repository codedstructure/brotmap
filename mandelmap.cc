// MandelMap - copyright Ben Bass 2010-2011

#include <cstdio>
#include <math.h>  // for NAN. Not in the C++ library...
#include <cstddef>
#include "mandelmap.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <pthread.h>

long mpoint(pinfo* p);

long inside_points = 0;

// these are the dimensions of the fractal we are plotting.
const FLOAT MIN_X = -2.0;
const FLOAT MAX_X = 1.0;
const FLOAT MIN_Y = -1.5;
const FLOAT MAX_Y = 1.5;
const FLOAT BAILOUT = 4.0;

inline long mpoint(FLOAT r,
                  FLOAT i,
                  pinfo* p,
                  int old_max_iter=0)
{
    if (isnan(p->x)) {
        // we've already processed this entry -
        // and it's not part of the Mset
        return 0;
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
        return 1;
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
        return 0;
    }
}

int INIT_MAX_ITER;
pinfo* FPTR_START;
FLOAT STEP_SIZE;
pthread_mutex_t acc_lock;

void* worker_start(void* arg)
{
    int t_num = *reinterpret_cast<int*>(arg);
    FLOAT t_start = t_num*STEP_SIZE;

    long local_inside = 0;

    // get the number of points to skip per row
    ptrdiff_t row_skip = (MAX_X-MIN_X)/STEP_SIZE;
    // get starting point in the mmapped file - t_num rows in
    pinfo* local_fptr = FPTR_START+(row_skip*t_num);

    for (FLOAT y=MIN_Y + t_start; y<MAX_Y; y+=(NUM_THREADS*STEP_SIZE))
    {
        for (FLOAT x=MIN_X; x<MAX_X; x+=STEP_SIZE)
        {
            pinfo* p=local_fptr++;
            local_inside += mpoint(y, x, p, MAX_ITER);
        }
        // skip ahead the relevant number of lines
        local_fptr += (row_skip * (NUM_THREADS-1));
    }
    pthread_mutex_lock(&acc_lock);
    inside_points += local_inside;
    pthread_mutex_unlock(&acc_lock);
}

int main()
{
    // for multiples of 2^-n (where n is < the number of mantissa digits)
    // floating point can exactly represent the values. Work out the
    // such a value by repeated dividing by two.
    //
    // TODO: this is all constant, so use a bit of TMP on it.
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

    pthread_t t_handle[NUM_THREADS];
    int t_num[NUM_THREADS];

    // setup global read-only vars //BADBADBAD (but could be worse)
    INIT_MAX_ITER = fheader->max_iter;
    FPTR_START = fptr;
    STEP_SIZE = step;

    printf("Using %d threads\n", NUM_THREADS);

    pthread_mutex_init(&acc_lock, NULL);
    for (int nt=0; nt < NUM_THREADS; nt++)
    {
        t_num[nt] = nt;
        pthread_create(&t_handle[nt],
                NULL, worker_start, reinterpret_cast<void*>(&t_num[nt]));
    }
    for (int nt=0; nt < NUM_THREADS; nt++)
    {
        pthread_join(t_handle[nt], NULL);
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
    printf("%llu %lu (%.6f)\n", total_points, inside_points, ((plot_area*inside_points)/float(total_points)));
}
