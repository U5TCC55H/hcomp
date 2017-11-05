#ifndef HUFFMAN_TREE
#define HUFFMAN_TREE

#include <boost/dynamic_bitset.hpp>

class HuffmanTree {
public:
    HuffmanTree(const float *freq, int num);
    const boost::dynamic_bitset<unsigned char> & encode(unsigned char ch);

private:
    void findmin(int &m1, int &m2);
    void merge(int m1, int m2);

    float *weight;
    int numChar;
    int *root;
    boost::dynamic_bitset<unsigned char> *code;
};

#endif
