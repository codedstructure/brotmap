#include "colourmap.h"

Colour colourIter(long iterCount, int max_iter, bool non_mset) {
    if (non_mset)
    {
        unsigned char zr = (iterCount * 5) % 255;
        unsigned char zg = ((iterCount + 85) / 2) % 255;
        unsigned char zb = ((iterCount + 172) * 3) % 255;

        return Colour(zr, zg, zb);
    }

    return Colour(0, 0, 0);
}
