#include "brotmap.h"
#include <iostream>
#include <stddef.h>
#include <pthread.h>

extern FLOAT MIN_X, MIN_Y, MAX_X, MAX_Y;
extern long inside_points;

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
            local_inside += mpoint(x, y, p, MAX_ITER);
        }
        // skip ahead the relevant number of lines
        local_fptr += (row_skip * (NUM_THREADS-1));
    }
    pthread_mutex_lock(&acc_lock);
    inside_points += local_inside;
    pthread_mutex_unlock(&acc_lock);
}

void worker_run(pinfo* fptr, const FLOAT step) {
    pthread_t t_handle[NUM_THREADS];
    int t_num[NUM_THREADS];

    // setup global read-only vars //BADBADBAD (but could be worse)
    FPTR_START = fptr;
    STEP_SIZE = step;

    std::cout << "Using " << NUM_THREADS << " threads" << std::endl;

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
};
