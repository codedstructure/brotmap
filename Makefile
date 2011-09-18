all: out.tiff

out.tiff: out.ppm
	ppm2tiff out.ppm out.tiff

out.ppm: make_ppm mandel.dat
	./make_ppm
  
mandel.dat: mandelmap
	time ./mandelmap

make_ppm: make_ppm.cc mandelmap.h
	g++ make_ppm.cc -o make_ppm
  
mandelmap: mandelmap.cc mandelmap.cc mandelmap.h
	g++ mandelmap.cc -Werror -O3 -o mandelmap -lc -lpthread
	
clean:
	rm -f mandelmap make_ppm out.ppm out.tiff

superclean: clean
	rm -f mandel_*.dat

