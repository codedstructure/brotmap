all: out.ppm


out.ppm: make_ppm mandel.dat
	./make_ppm
  
mandel.dat: mandelmap
	./mandelmap

make_ppm: make_ppm.cc mandelmap.h
	g++ make_ppm.cc -o make_ppm
  
mandelmap: mandelmap.cc mandelmap.cc mandelmap.h
	g++ mandelmap.cc -fast -o mandelmap -lc
	
clean:
	rm -f mandelmap make_ppm out.ppm mandel.dat
