all: out_image

out_image: out.ppm
	which pnmtopng > /dev/null && pnmtopng out.ppm > out_image || ppm2tiff out.ppm out_image

out.ppm: make_ppm mandel.dat
	./make_ppm
  
mandel.dat: brotmap
	time ./brotmap

make_ppm: make_ppm.cc brotmap.h
	g++ make_ppm.cc -o make_ppm
  
brotmap: brotmap.cc brotmap.h
	g++ brotmap.cc worker.cc evaluate.cc -Werror -O3 -o brotmap -lc -lpthread
	
clean:
	rm -f brotmap make_ppm out.ppm out_image

superclean: clean
	rm -f mandel_*.dat
