#include <stdio.h>
#include <math.h>
#include "brotmap.h"


int main(int argc, char *argv[])
{
    pinfo indata;
    brotfile_header fheader;

    if (argc < 2) {
        printf("Usage: make_ppm.cc input file");
        return 2;
    }

    char* fname = argv[1];
    FILE* inf = fopen(fname, "rb");
    fread(&fheader, sizeof(brotfile_header), 1, inf);
    printf("max_iter: %d\n", fheader.max_iter);

    const int BINARY_DIGITS = fheader.binary_digits;
    const int SIZE = (sizeof(pinfo)*3*(1<<BINARY_DIGITS-4));

    FILE* outf = fopen("out.ppm", "wb");
    fprintf(outf, "P6\n%d\n%d\n255\n", SIZE, SIZE);
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
