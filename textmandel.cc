#include <stdio.h>
#include "mandelmap.h"

int mpoint(FLOAT r, FLOAT i, int maxit)
{
    FLOAT x2, y2;
    FLOAT x = r;
    FLOAT y = i;
    int k = maxit;
    do {
        x2 = x*x;
        y2 = y*y;
        y = 2*x*y+i;
        x = x2-y2 + r;
    } while (((x2 + y2) < 4.0)  && --k);

    return maxit - k;
}

const int MAX_X = 80;
const int MAX_Y = 40;

int main()
{
    for (int row=0; row<MAX_Y; row++)
    {
        for (int col=0; col<MAX_X; col++)
        {
            if (mpoint(col/(MAX_X/3.0) - 1.5,
                       row/(MAX_Y/3.0) - 1.5,
                       MAX_ITER) == MAX_ITER)
                putchar('*');
            else
                putchar(' ');
        }
        putchar('\n');
    }
}
