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

void compress(const char *ifname, const char *ofname) {
    // 打开文件
    int fd = open(ifname, O_RDONLY, 0);
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
    // 统计频率
    float *freq = new float[256];
    for (int i = 0; i < 256; ++i)
        freq[i] = 0;
    for (int i = 0;  i < st.st_size; ++i)
        ++freq[buff[i]];
    for (int i = 0; i < 256; ++i)
        freq[i] /= st.st_size;
    // 根据freq构造Huffman树
    HuffmanTree tree(freq, 256);
    // 编码
    BitStream obs;
    for (int i = 0; i < st.st_size; ++i) {
        obs << tree.encode(buff[i]);
    }
    munmap(buff, st.st_size);
    // 打开文件
    ofstream fout(ofname, ios::binary);
    // 保存频率表
    fout.write((char*)freq, sizeof(float) * 256);
    delete[] freq;
    // 保存padding
    int padding = (8-obs.getNumBit() % 8)%8;
    fout.write((char*)&padding, sizeof(int));
    // 保存压缩后的比特流
    unsigned char *bufff = new unsigned char[obs.getNumElem()];
    for (int i = 0; i < obs.getNumElem(); ++i)
        bufff[i] = obs[i];
    fout.write((char*)bufff, sizeof(unsigned char) * obs.getNumElem());
    delete[] bufff;
    fout.close();
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
    HuffmanTree tree((float*)buff, 256);
    // 构造比特流
    boost::dynamic_bitset<unsigned char> bs(buff + sizeof(float)*256 + sizeof(int), buff+st.st_size);
    int padding = *(int*)(buff+sizeof(float)*256);
    munmap(buff, st.st_size);    
    // 去除padding
    bs.resize(bs.size()-padding);
    // 解压缩，非常耗时
    BitStream obs;
    tree.decode(bs, obs);
    // 输出到文件
    buff = new unsigned char[obs.getNumElem()];
    for (int i = 0; i < obs.getNumElem(); ++i)
        buff[i] = obs[i];
    ofstream fout(foutname, ios::binary);
    fout.write((char*)buff, obs.getNumElem());
    fout.close();
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        cout << "Usage: " << argv[0] << " c|d file_name1 file_name2" << endl;
        exit(-1);
    }
    switch (argv[1][0]) {
        case 'c':
            compress(argv[2], argv[3]);
            break;
        case 'd':
            decompress(argv[2], argv[3]);
            break;
        default:
            cout << "Unknown option" << endl;
    }
    return 0;
}
