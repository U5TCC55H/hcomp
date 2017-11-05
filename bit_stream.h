#ifndef BIT_STREAM
#define BIT_STREAM

#include <fstream>
using namespace std;

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

#include <boost/dynamic_bitset.hpp>

class BitStream {
public:
    BitStream(int elemSize = 8);
    ~BitStream() {
        if (bitset) {
            delete bitset;
            bitset = nullptr;
        }
    }
    bool fromFile(const char *fname);
    unsigned char operator[](int idx) const;
    int getNumElem() const {
        return bitset == nullptr? -1 : (bitset->size() + elemSize - 1)/elemSize;
    }
    int getNumBit() const {
        return bitset == nullptr? -1 : bitset->size();
    }
    BitStream & operator<<(const boost::dynamic_bitset<unsigned char> &bs);
    BitStream & operator<<(unsigned char);
    friend ostream & operator<<(ostream &os, const BitStream &bs) {
        return os << *(bs.bitset);
    }
private:
    int elemSize;
    int seek;
    boost::dynamic_bitset<unsigned char> *bitset;
    int buff_len;
};

#endif
