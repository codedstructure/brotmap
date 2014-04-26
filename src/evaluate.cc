#include "brotmap.h"
#include <math.h>  // for NAN. Not in the C++ library...

const FLOAT BAILOUT = 4.0;

// mpoint(real, image, ...) -> 1 if mset else 0
long mpoint(FLOAT r,
            FLOAT i,
            pinfo* p,
            int old_max_iter)
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

#ifdef CARDIOID_OPT
    // based on 'Optimizations' section in wikipedia
    // http://en.wikipedia.org/wiki/Mandelbrot_set#Optimizations
    double roff = (r-0.25);
    double isq = i*i;
    double dist = roff*roff + isq;
    if (dist * (dist + roff) < 0.25*isq)
    {
        return 1;
    }

    roff = r+1.0;
    if (roff*roff + isq < 1.0/16.0)
    {
        return 1;
    }
#endif
    // this is the core 'inner loop'.
    // It should be fast, which is why it doesn't
    // have lots of whitespace in it ;)
    //
    // of course this is massively parallelizable
    do {
        x2 = x*x;
        y2 = y*y;
        if ((x2+y2) >= BAILOUT) break;
        y = 2.0*x*y+i;
        x = x2-y2+r;
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
