#include <stdio.h>
#include <math.h>
#include "mandelmap.h"

const int SIZE= (sizeof(pinfo)*3*(1<<BINARY_DIGITS-4));

int main()
{
    FILE* outf = fopen("out.ppm", "wb");
    FILE* inf = fopen("mandel.dat", "rb");

    pinfo indata;

    fprintf(outf, "P6\n%d\n%d\n255\n", SIZE, SIZE);
    //fseek(inf, (row*SIZE+col)*sizeof(pinfo), SEEK_SET);
    for (int row=0; row<SIZE; row++)
    {
        for (int col=0; col<SIZE; col++)
        {
            fread(&indata, sizeof(pinfo), 1, inf);
            if (isnan(indata.x))
            {
                unsigned char zz = (MAX_ITER - indata.itercount) % 256;
                putc(zz, outf);
                putc(zz, outf);
                putc(zz, outf);
            }
            else
            {
                putc(0, outf);
                putc(0, outf);
                putc(0, outf);
            }
        }
    }
}
