#ifndef HUFFMAN_TREE
#define HUFFMAN_TREE

#include <boost/dynamic_bitset.hpp>
#include "bit_stream.h"

class HuffmanTree {
public:
    HuffmanTree(const float *freq, int num);
    ~HuffmanTree() {
        delete[] weight;
        delete[] code;
        delete[] childL;
        delete[] childR;
    }
    const boost::dynamic_bitset<unsigned char> & encode(unsigned char ch);
    void decode(const boost::dynamic_bitset<unsigned char> &bs, BitStream &obs);
    int find_match(boost::dynamic_bitset<unsigned char> bs);
private:
    void findmin(int &m1, int &m2);
    void merge(int m1, int m2);

    int numChar;
    float *weight;
    int *root;
    int oracle;
    int *childL;
    int *childR;
    boost::dynamic_bitset<unsigned char> *code;
};

#endif
