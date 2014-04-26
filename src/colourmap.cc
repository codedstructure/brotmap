#include "colourmap.h"

Colour colourIter(long iterCount, int max_iter, bool non_mset) {
    if (non_mset)
    {
        unsigned char zz = iterCount % 255;
        return Colour({.r=zz, .g=zz, .b=zz});
    }
    else
    {
        return Colour({.r=0, .g=0, .b=0});
    }
}
