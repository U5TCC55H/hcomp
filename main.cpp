#include <iostream>
using namespace std;
#include <boost/dynamic_bitset.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

#include "huffman_tree.h"

/*
    设置元素大小
    从文件生成
    读出固定的一个元素
*/
class BitStream {
public:
    BitStream(int elemSize = 8);
    bool fromFile(const char *fname);
    unsigned char operator[](int idx);
    int getNumElem() {
        return bitset == nullptr? -1 : bitset->size()/elemSize;
    }
private:
    int elemSize;
    int seek;
    boost::dynamic_bitset<unsigned char> *bitset;
    int buff_len;
};

BitStream::BitStream(int elemSize)
    : elemSize(8), seek(0), bitset(nullptr)
{
    if (elemSize > 0 && elemSize <= 8)
        this->elemSize = elemSize;
}

// 从文件读取比特流，失败返回false
bool BitStream::fromFile(const char *fname) {
    bitset = nullptr;
    // 打开文件
    int fd = open(fname, O_RDONLY, 0);
    if (fd == -1)
        return false;
    // 得到文件大小
    struct stat st;
    if(fstat(fd,&st))
        return false;
    // 映射到内存
    unsigned char *buff = (unsigned char*)mmap(NULL,st.st_size,PROT_READ,MAP_SHARED,fd,0);
    if (buff == (void*)-1) {
        buff = NULL;
        return false;
    }
    // 构造比特流
    bitset = new boost::dynamic_bitset<unsigned char>(buff, buff+st.st_size);

    munmap(buff, st.st_size);
    return true;
}

// 得到第idx个elem
unsigned char BitStream::operator[](int idx) {
    unsigned char ret = 0;
    for (int i = 0; i < elemSize; ++i)
        ret = ret | ((*bitset)[elemSize*idx+i] << i);
    return ret;
}

void compress(const char *ifname, const char *ofname, int elemSize) {
    const int csz = 1 << elemSize;
    // 读取比特流
    BitStream bs(elemSize);
    if (!bs.fromFile("input.dat")) {
        cerr << "failed to open file" << endl;
    }
    int numElem = bs.getNumElem();    
    // 统计频率
    float *freq = new float[csz];
    for (int i = 0; i < csz; ++i)
    freq[i] = 0;
    for (int i = 0; i < numElem; ++i)
        ++freq[bs[i]];
    for (int i = 0; i < csz; ++i)
        freq[i] /= numElem;
    // 根据freq构造Huffman树
    HuffmanTree tree(freq, numElem);
    // 编码并保存
    BitStream obs;
    for (int i = 0; i < numElem; ++i)
        obs << tree.encode(bs[i]);
    save(ofname, elemSize, freq, obs)

    delete[] freq;
}

void save(const char *fname, int elemSize, float *freq, const BitStream &bs) {
}

int main() {
    compress("input.dat", "output.dat", 4);
    return 0;
}