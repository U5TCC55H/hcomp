#include <iostream>
#include <fstream>
using namespace std;
#include <boost/dynamic_bitset.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

#include "bit_stream.h"
#include "huffman_tree.h"

void save(const char *fname, int elemSize, float *freq, const BitStream &bs) {
    ofstream fout(fname, ios::binary);
    // 保存压缩粒度
    fout.write((char*)&elemSize, sizeof(int));
    // 保存频率表
    fout.write((char*)freq, sizeof(float) * (1 << elemSize));
    // 保存padding
    int padding = (8-bs.getNumBit() % 8)%8;
    fout.write((char*)&padding, sizeof(int));
    // 保存压缩后的比特流
    unsigned char *buff = new unsigned char[bs.getNumElem()];
    for (int i = 0; i < bs.getNumElem(); ++i)
        buff[i] = bs[i];
    fout.write((char*)buff, sizeof(unsigned char) * bs.getNumElem());
    delete[] buff;
    fout.close();
}

void compress(const char *ifname, const char *ofname, int elemSize) {
    const int numChar = 1 << elemSize;
    // 读取比特流
    BitStream bs(elemSize);
    if (!bs.fromFile(ifname)) {
        cerr << "failed to open file" << endl;
    }
    int numElem = bs.getNumElem();
    // 统计频率
    float *freq = new float[numChar];
    for (int i = 0; i < numChar; ++i)
        freq[i] = 0;
    for (int i = 0;  i < numElem; ++i)
        ++freq[bs[i]];
    for (int i = 0; i < numChar; ++i)
        freq[i] /= numElem;
    // 根据freq构造Huffman树
    HuffmanTree tree(freq, numChar);
    // 编码并保存
    BitStream obs;
    for (int i = 0; i < numElem; ++i) {
        obs << tree.encode(bs[i]);
    }
    save(ofname, elemSize, freq, obs);

    delete[] freq;
}

void decompress(const char *fname, const char *foutname) {
    // 打开文件
    int fd = open(fname, O_RDONLY, 0);
    if (fd == -1)
        return;
    // 得到文件大小
    struct stat st;
    if(fstat(fd,&st))
        return;
    // 映射到内存
    unsigned char *buff = (unsigned char*)mmap(NULL,st.st_size,PROT_READ,MAP_SHARED,fd,0);
    if (buff == (void*)-1) {
        buff = NULL;
        return;
    }
    int numChar = 1 << *(int*)buff;
    HuffmanTree tree((float*)(buff+sizeof(int)), numChar);
    // 构造比特流
    boost::dynamic_bitset<unsigned char> bs(buff+sizeof(int)+sizeof(float)*numChar+sizeof(int), buff+st.st_size);
    int padding = *(int*)(buff+sizeof(int)+sizeof(float)*numChar);
    munmap(buff, st.st_size);    
    bs.resize(bs.size()-padding);
    // 解压缩
    BitStream obs;
    tree.decode(bs, obs);
    //
    buff = new unsigned char[obs.getNumElem()];
    for (int i = 0; i < obs.getNumElem(); ++i)
        buff[i] = obs[obs.getNumElem() - 1 - i];
    ofstream fout(foutname, ios::binary);
    cout << obs.getNumElem();
    fout.write((char*)buff, obs.getNumElem());
    fout.close();
}

int main() {
    compress("t_input.dat", "t_output.dat", 8);
    decompress("t_output.dat", "t_decompressed.dat");
    return 0;
}
