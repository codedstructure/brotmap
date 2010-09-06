#include <stdio.h>
#include <math.h> // for NAN
#include "mandelmap.h"

int mpoint(pinfo* p);

int inside = 0;

int mpoint(pinfo* p)
{
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
    for (int i=0; i<BINARY_DIGITS; i++) step /=2;

    FILE* f = fopen("mandel.dat", "wb");

    unsigned long long z =0;

    for (FLOAT y=-1.5; y<1.5; y+=step)
    {
        for (FLOAT x=-2.0; x<1.0; x+=step)
        {
            pinfo p;
            p.x = x;
            p.y = y;
            mpoint(&p);
            fwrite(&p, sizeof(pinfo), 1, f);
            z ++;
        }
    }

    printf("%llu %d\n", z, inside);
}
