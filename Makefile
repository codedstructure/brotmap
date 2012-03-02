all: out_image

out_image: out.ppm
	which pnmtopng > /dev/null && pnmtopng out.ppm > out_image || ppm2tiff out.ppm out_image

out.ppm: make_ppm mandel.dat
	./make_ppm
  
mandel.dat: mandelmap
	time ./mandelmap

make_ppm: make_ppm.cc mandelmap.h
	g++ make_ppm.cc -o make_ppm
  
mandelmap: mandelmap.cc mandelmap.cc mandelmap.h
	g++ mandelmap.cc -Werror -O3 -o mandelmap -lc -lpthread
	
clean:
	rm -f mandelmap make_ppm out.ppm out_image

superclean: clean
	rm -f mandel_*.dat
