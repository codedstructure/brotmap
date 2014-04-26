#ifndef COLOURMAP_H
#define COLOURMAP_H

struct Colour {
    unsigned char r, g, b;

    Colour(unsigned char r, unsigned char g, unsigned char b) :
        r(r), g(g), b(b)
    {}
};

Colour colourIter(long iterCount, int max_iter, bool non_mset);

#endif
