all: out_image

out_image: out.ppm
	which pnmtopng > /dev/null && pnmtopng out.ppm > out_image || ppm2tiff out.ppm out_image

out.ppm: make_ppm mandel.dat
	./make_ppm mandel.dat
  
mandel.dat: brotmap
	rm -f x.dat && time ./brotmap mandel.dat 10

make_ppm: make_ppm.cc brotmap.h
	g++ --std=c++11 make_ppm.cc -o make_ppm
  
brotmap: brotmap.cc worker.cc evaluate.cc brotmap.h
	g++ --std=c++11 brotmap.cc worker.cc evaluate.cc -Werror -O3 -o brotmap -lc -lpthread
	
clean:
	rm -f brotmap mandel.dat make_ppm out.ppm out_image

superclean: clean
	rm -f mandel*.dat
