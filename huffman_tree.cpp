#include "huffman_tree.h"
#include <iostream>
using namespace std;
#include <string.h>

HuffmanTree::HuffmanTree(const float *freq, int num)
    : numChar(num)
{
    weight = new float[num];
    root = new int[num];
    for (int i = 0; i < num; ++i)
    {
        root[i] = i;
        weight[i] = freq[i];
    }
    code = new boost::dynamic_bitset<unsigned char>[num];
    memcpy(weight, freq, sizeof(float) * num);
    for (int _ = 0; _ < num - 1; ++_)
    {
        int m1, m2;
        findmin(m1, m2); // 找到权最小的两个根
        merge(m1, m2);   // 合并，包括更新编码
    }
    memcpy(weight, freq, sizeof(float) * num);

    delete[] root;
}

const boost::dynamic_bitset<unsigned char> &HuffmanTree::encode(unsigned char ch)
{
    return code[ch];
}

void HuffmanTree::findmin(int &m1, int &m2)
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
            // 延长编码
            code[i].push_back(false);
            code[i] <<= 1;
            code[i][0] = false;
        }
        else if (root[i] == m2)
        {
            root[i] = m1;
            // 延长编码
            code[i].push_back(false);
            code[i] <<= 1;
            code[i][0] = true;
        }
    }
    weight[m1] += weight[m2];
}

void HuffmanTree::decode(const boost::dynamic_bitset<unsigned char> &bs, BitStream &obs) {
    unsigned int idx = 0;
    boost::dynamic_bitset<unsigned char> tmp;
    while (idx < bs.size())
    {
        tmp.push_back(bs[idx]);
        int seek = find_match(tmp);
        if (seek != -1)
        {
            obs << (unsigned char)seek;
            tmp.resize(0);
        }
        ++idx;
    }
}

int HuffmanTree::find_match(boost::dynamic_bitset<unsigned char> bs)
{
    for (int i = 0; i < numChar; ++i)
    {
        if (code[i] == bs)
            return i;
    }
    return -1;
}
