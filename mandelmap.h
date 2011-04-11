// MandelMap - copyright Ben Bass 2010

#ifndef MANDEL_MAP_H
#define MANDEL_MAP_H

typedef double FLOAT;

const int BINARY_DIGITS = 10;

const int MAX_ITER=256;

struct pinfo
{
    FLOAT x;
    union {
      FLOAT y;
      long itercount;
    };
};


const int HEADER_LEN=4096;
struct brotfile_header
{
    FLOAT startx;
    FLOAT starty;
    int binary_size;
    int max_iter;
};


#endif
