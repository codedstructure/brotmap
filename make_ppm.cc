#include <stdio.h>
#include <math.h>
#include "mandelmap.h"

const int SIZE= (sizeof(pinfo)*3*(1<<BINARY_DIGITS-4));

int main()
{
    FILE* outf = fopen("out.ppm", "wb");
    char fname[21];
    snprintf(fname, 20, "mandel_%d.dat", BINARY_DIGITS);
    FILE* inf = fopen(fname, "rb");

    pinfo indata;
    brotfile_header fheader;

    fprintf(outf, "P6\n%d\n%d\n255\n", SIZE, SIZE);
    fread(&fheader, sizeof(brotfile_header), 1, inf);
    printf("max_iter: %d\n", fheader.max_iter);
    fseek(inf, HEADER_LEN, SEEK_SET);
    for (int row=0; row<SIZE; row++)
    {
        for (int col=0; col<SIZE; col++)
        {
            fread(&indata, sizeof(pinfo), 1, inf);
            if (isnan(indata.x))
            {
                unsigned char zz = (fheader.max_iter - indata.itercount) % 256;
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
