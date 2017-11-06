flags=-lprofiler -O3 -g -Wall

main: main.o bit_stream.o huffman_tree.o
	g++ ${flags} -o $@ $^

%.o: %.cpp
	g++ ${flags} -c $^

clean:
	rm *.o
	rm main

test: test_correct test_memory

test_correct: main
	bash -c 'time ./main c t_input.dat t_output.dat'
	bash -c 'time ./main d t_output.dat t_decompressed.dat'
	cmp t_input.dat t_decompressed.dat

test_memory: main
	valgrind --leak-check=full ./main c t_input.dat t_output.dat

prof: main
	bash -c 'CPUPROFILE=prof.out ./main c ./t_input.dat ./t_output.dat'
	pprof -gv ./main prof.out
	bash -c 'CPUPROFILE=prof.out ./main d ./t_output.dat ./t_decompressed.dat'
	pprof -gv ./main prof.out
