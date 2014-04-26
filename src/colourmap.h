#ifndef COLOURMAP_H
#define COLOURMAP_H

struct Colour {
    unsigned char r, g, b;
};

Colour colourIter(long iterCount, int max_iter, bool non_mset);

#endif
