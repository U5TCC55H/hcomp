#include "bit_stream.h"

BitStream::BitStream(int elemSize)
    : elemSize(8), seek(0), bitset(nullptr)
{
    if (elemSize > 0 && elemSize <= 8)
        this->elemSize = elemSize;
}

// 从文件读取比特流，失败返回false
bool BitStream::fromFile(const char *fname)
{
    bitset = nullptr;
    // 打开文件
    int fd = open(fname, O_RDONLY, 0);
    if (fd == -1)
        return false;
    // 得到文件大小
    struct stat st;
    if (fstat(fd, &st))
        return false;
    // 映射到内存
    unsigned char *buff = (unsigned char *)mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (buff == (void *)-1)
    {
        buff = NULL;
        return false;
    }
    // 构造比特流
    bitset = new boost::dynamic_bitset<unsigned char>(buff, buff + st.st_size);

    munmap(buff, st.st_size);
    return true;
}

// 得到第idx个elem
unsigned char BitStream::operator[](int idx) const
{
    unsigned char ret = 0;
    for (int i = 0; i < elemSize; ++i)
        ret = ret | ((*bitset)[elemSize * idx + i] << i);
    return ret;
}

BitStream &BitStream::operator<<(const boost::dynamic_bitset<unsigned char> &bs)
{
    if (bitset == nullptr)
        bitset = new boost::dynamic_bitset<unsigned char>;
    for (unsigned int i = 0; i < bs.size(); ++i)
    {
        bitset->push_back(bs[i]);
    }
    return *this;
}

BitStream &BitStream::operator<<(unsigned char ch)
{
    if (bitset == nullptr)
        bitset = new boost::dynamic_bitset<unsigned char>;
    bitset->append(ch);
    return *this;
}