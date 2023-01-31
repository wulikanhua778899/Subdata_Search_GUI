#pragma once
#include <fstream>

class Kmp
{
private:
    std::ifstream* pfin = nullptr;
    int fileLen;
    int offset;

    // scrStr不分配内存
    char* srcStr = nullptr;
    int srcLen;
    // matchStr和kmpNext分配内存
    char* matchStr = nullptr;
    int matchLen;
    int* kmpNext = nullptr;
    int i, j;

    void generateNext(char*, int);
    int strSearching(int, int);
    int streamSearching(int, int);

public:
    // 输入模式串和原串并查找第一个索引
    int run(char*, char*, int, int);
    // 通过已定义的模式串查找原串索引
    int run(char*, int);
    // 继续查找模式串在原串中的索引
    int run();

    // 输入模式串和原串并查找第一个索引
    int frun(std::ifstream*, char*, int, int m_offset = 0, int m_fileLen = INT_MAX);
    // 通过已定义的模式串查找原串索引
    int frun(std::ifstream*, int m_offset = 0, int m_fileLen = INT_MAX);
    // 继续查找模式串在原串中的索引
    int frun();

    Kmp(const Kmp&);
    Kmp(char* match, int m_matchLen) { generateNext(match, m_matchLen); };
    ~Kmp()
    {
        if (kmpNext)
        {
            delete[] kmpNext;
            kmpNext = NULL;
        }
        if (matchStr)
        {
            delete[] matchStr;
            matchStr = NULL;
        }
    };
};

