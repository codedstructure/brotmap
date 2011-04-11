// MandelMap - copyright Ben Bass 2010

#ifndef MANDEL_MAP_H
#define MANDEL_MAP_H

typedef double FLOAT;

const int BINARY_DIGITS = 12;

const int MAX_ITER=100;

struct pinfo
{
    FLOAT x;
    union {
      FLOAT y;
      long itercount;
    };
};

#endif
