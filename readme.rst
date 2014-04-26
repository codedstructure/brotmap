
brotmap
-------

*Precalculating the Mandelbrot set - because it's there*

This is mostly a learning exercise for me.

Key things:
 - use of anonymous unions and NAN for efficient storage of the
   mandel_X.dat files (depite the fact that they are rather large still...)
   This was my original 'cool idea' / excuse for starting this.
 - use of mmap in a 64 bit address space for basically *doing anything*.
   Nice.
 - use of pthreads to utilise multi-core processors

And of course, the key thing:
 - using a computer for it's intended purpose: computing, taking all the
   resources it possibly can in the process :-)

For the future:
 - distributed parallelism
 - not only incremental in terms of iterations, but also incremental in
   spatial resolution (which will probably require a change in the file
   format, or something like a group of 4 mandel_X.dats  which are
   postprocessed to create a mandel_(X+1).dat, by storing an offset
   of zero or 2^-(X+1) in each of the real and imaginary axes...
 - an actual command line interface, with options and everything


This is unlicensed software, see UNLICENSE for details.
Created by Ben Bass 2010-2014 @codedstructure
