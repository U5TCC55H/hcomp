main: main.o bit_stream.o huffman_tree.o
	g++ -O3 -g -Wall -o $@ $^

%.o: %.cpp
	g++ -O3 -g -c $^

clean:
	rm *.o
	rm main

test: test_result test_memory

test_result:
	./main c t_input.dat t_output.dat
	./main d t_output.dat t_decompressed.dat
	cmp t_input.dat t_decompressed.dat

test_memory:
	valgrind --leak-check=full ./main c t_input.dat t_output.dat
