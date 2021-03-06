// MandelMap - Ben Bass 2010-2014

#ifndef MANDEL_MAP_H
#define MANDEL_MAP_H

#define CARDIOID_OPT

typedef double FLOAT;

const int NUM_THREADS = 2;
const int MAX_ITER=1024;

struct pinfo
{
    FLOAT x;
    union {
      FLOAT y;
      long itercount;
    };
};

// check that y and itercount are the same size
static_assert(sizeof(FLOAT) == sizeof(long), "Invalid sizeof spec");

const int HEADER_LEN=4096;  // probable page size for happiness
struct brotfile_header
{
    FLOAT startx;
    FLOAT starty;
    int binary_digits;
    int max_iter;
};

long mpoint(FLOAT r,
            FLOAT i,
            pinfo* p,
            int old_max_iter);
void* worker_start(void* arg);
void worker_run(pinfo* fptr, const FLOAT step, int old_max_iter);

#endif
