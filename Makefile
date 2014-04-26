all: out_image

out_image: out.ppm
	which pnmtopng > /dev/null && pnmtopng out.ppm > out_image || ppm2tiff out.ppm out_image

out.ppm: make_ppm mandel.dat
	./make_ppm mandel.dat
  
mandel.dat: brotmap
	rm -f x.dat && time ./brotmap mandel.dat 10

make_ppm: src/make_ppm.cc src/brotmap.h
	g++ --std=c++11 src/make_ppm.cc -o make_ppm
  
brotmap: src/brotmap.cc src/worker.cc src/evaluate.cc src/brotmap.h
	g++ --std=c++11 src/brotmap.cc src/worker.cc src/evaluate.cc -Werror -O3 -o brotmap -lc -lpthread
	
clean:
	rm -f brotmap mandel.dat make_ppm out.ppm out_image

superclean: clean
	rm -f mandel*.dat
