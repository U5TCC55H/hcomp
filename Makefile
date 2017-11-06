flags=-O3 -Wall -lprofiler

hcomp: main.o bit_stream.o huffman_tree.o
	g++ ${flags} -o $@ $^

%.o: %.cpp
	g++ ${flags} -c $^

clean:
	rm -f *.o hcomp

test: hcomp
	bash ./test.sh t_input_zero.dat
	bash ./test.sh t_input_random.dat
	bash ./test.sh t_input_sda1.dat

test_memory: hcomp
	valgrind --leak-check=full --show-leak-kinds=all ./hcomp c t_input.dat t_output.dat

prof: hcomp
	bash -c 'CPUPROFILE=prof.out ./hcomp c ./t_input.dat ./t_output.dat'
	pprof -gv ./hcomp prof.out
	bash -c 'CPUPROFILE=prof.out ./hcomp d ./t_output.dat ./t_decompressed.dat'
	pprof -gv ./hcomp prof.out
	rm prof.out
