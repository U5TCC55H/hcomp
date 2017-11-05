main: main.cpp huffman_tree.cpp bit_stream.cpp
	g++ -O3 -g -Wall -o $@ $^

test:
	./main c t_input.dat t_output.dat
	./main d t_output.dat t_decompressed.dat
	cmp t_input.dat t_decompressed.dat
