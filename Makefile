main: main.cpp huffman_tree.cpp bit_stream.cpp
	g++ -O3 -g -Wall -o $@ $^
