#include "brotmap.h"
#include <cstdio>
#include <cmath>
#include <string>

using namespace std;

class BrotMapFile {
    public:
        BrotMapFile (string filename);

        int getMaxIter() const;
        int getBinaryDigits() const;
        int getImageSize() const;

        pinfo nextPoint();

    private:
        FILE* bfile;
        brotfile_header fheader;
};

BrotMapFile::BrotMapFile(string filename) {
    bfile = fopen(filename.c_str(), "rb");
    fread(&fheader, sizeof(brotfile_header), 1, bfile);
    // we reserve some space for the header, need to skip that
    fseek(bfile, HEADER_LEN, SEEK_SET);
}

int BrotMapFile::getMaxIter() const {
    return fheader.max_iter;
}

int BrotMapFile::getBinaryDigits() const {
    return fheader.binary_digits;
}

int BrotMapFile::getImageSize() const {
    return (sizeof(pinfo)*3*(1<<(getBinaryDigits()-4)));
}

pinfo BrotMapFile::nextPoint() {
    pinfo indata;
    fread(&indata, sizeof(pinfo), 1, bfile);
    return indata;
};

int main(int argc, char *argv[])
{
    pinfo indata;

    if (argc < 3) {
        printf("Usage: make_ppm.cc input_file output_file\n");
        return 2;
    }

    BrotMapFile bmf(argv[1]);

    printf("max_iter: %d\n", bmf.getMaxIter());

    int image_size = bmf.getImageSize();
    int max_iter = bmf.getMaxIter();

    FILE* outf = fopen(argv[2], "wb");
    fprintf(outf, "P6\n%d\n%d\n255\n", image_size, image_size);
    for (int pix=0; pix < (image_size * image_size); pix++)
    {
        indata = bmf.nextPoint();
        if (isnan(indata.x))
        {
            unsigned char zz = (max_iter - indata.itercount) % 256;
            putc(zz, outf);
            putc(zz, outf);
            putc(zz, outf);
        }
        else
        {
            putc(100, outf);
            putc(0, outf);
            putc(0, outf);
        }
    }
}
