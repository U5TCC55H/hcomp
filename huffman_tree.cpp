#include "huffman_tree.h"
#include <iostream>
using namespace std;
#include <string.h>

HuffmanTree::HuffmanTree(const float *freq, int num)
    : numChar(num)
{
    weight = new float[num*2];
    root = new int[num*2];
    childL = new int[num*2];
    childR = new int[num*2];
    oracle = num;
    for (int i = 0; i < num; ++i)
    {
        weight[i] = freq[i];
        root[i] = i;
        childL[i] = childR[i] = -1;
    }
    code = new boost::dynamic_bitset<unsigned char>[num];
    for (int _ = 0; _ < num - 1; ++_)
    {
        int m1, m2;
        findMin(m1, m2); // 找到权最小的两个根
        merge(m1, m2);   // 合并，包括更新编码
    }
    --oracle;
    memcpy(weight, freq, sizeof(float) * num);

    delete[] root;
}

const boost::dynamic_bitset<unsigned char> &HuffmanTree::encode(unsigned char ch)
{
    return code[ch];
}

// 考虑到树的大小，这点开销微不足道
void HuffmanTree::findMin(int &m1, int &m2)
{
    m1 = root[0];
    for (int i = 0; i < numChar; ++i)
    {
        int tmp = root[i];
        if (weight[tmp] < weight[m1])
            m1 = tmp;
    }
    m2 = m1;
    for (int i = 0; i < numChar; ++i)
    {
        int tmp = root[i];
        if (m2 == m1 || (tmp != m1 && weight[tmp] < weight[m2]))
            m2 = tmp;
    }
}

void HuffmanTree::merge(int m1, int m2)
{
    // 更新节点以及其编码
    for (int i = 0; i < numChar; ++i)
    {
        if (root[i] == m1)
        {
            root[i] = oracle;
            // 延长编码
            code[i].push_back(false);
            code[i] <<= 1;
            code[i][0] = false;
        }
        else if (root[i] == m2)
        {
            root[i] = oracle;
            // 延长编码
            code[i].push_back(false);
            code[i] <<= 1;
            code[i][0] = true;
        }
    }
    weight[oracle] = weight[m1] + weight[m2];
    root[oracle] = oracle;
    childL[oracle] = m1;
    childR[oracle] = m2;
    ++oracle;
}

void HuffmanTree::decode(const boost::dynamic_bitset<unsigned char> &bs, unsigned char *buff) {
    unsigned int seek = 0;
    for (unsigned int i = 0, cur = oracle; i < bs.size(); ++i) {
        cur = bs[i]? childR[cur] : childL[cur];
        if (childL[cur] == -1 && childR[cur] == -1) {
            buff[seek++] = (unsigned char)cur;
            cur = oracle;
        }
    }
}

int HuffmanTree::findMatch(boost::dynamic_bitset<unsigned char> bs)
{
    for (int i = 0; i < numChar; ++i)
    {
        if (code[i] == bs)
            return i;
    }
    return -1;
}
