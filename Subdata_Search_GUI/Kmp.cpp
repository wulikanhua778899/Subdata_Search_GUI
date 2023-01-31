#include "pch.h"
#include "Kmp.h"
#include <iostream>
#include <cstring>

// 生成Next数组
void Kmp::generateNext(char* match, int m_matchLen)
{
	matchLen = m_matchLen;
	// 重新生成Next数组
	if (kmpNext)
		delete[] kmpNext;
	kmpNext = new int[matchLen + 1];

	// 重新保存模式串
	if (matchStr)
		delete[] matchStr;
	matchStr = new char[matchLen];
	memcpy(matchStr, match, matchLen);

	int i, j;
	i = 0;
	j = kmpNext[0] = -1;

	while (i < matchLen)
	{
		while (j > -1 && matchStr[i] != matchStr[j])
			j = kmpNext[j];
		i++;
		j++;
		if (matchStr[i] == matchStr[j])
			kmpNext[i] = kmpNext[j];
		else
			kmpNext[i] = j;
	}
}

// 在数组串中查找模式串
int Kmp::strSearching(int i, int j)
{
	while (j < srcLen)
	{
		while (i > -1 && matchStr[i] != srcStr[j])
			i = kmpNext[i];
		i++;
		j++;
		if (i >= matchLen)
		{
			this->i = kmpNext[i];
			this->j = j;
			return j - i;
		}
	}
	return -1;
}

int Kmp::run(char* src, char* match, int m_srcLen, int m_matchLen)
{
	srcStr = src;
	srcLen = m_srcLen;

	/* Preprocessing */
	generateNext(match, m_matchLen);

	/* Searching */
	return strSearching(0, 0);
}

int Kmp::run(char* src, int m_srcLen)
{
	if (!(kmpNext && matchStr))
		return -1;

	srcStr = src;
	srcLen = m_srcLen;

	return strSearching(0, 0);
}

int Kmp::run()
{
	if (!(kmpNext && matchStr) && !srcStr)
		return -1;

	return strSearching(this->i, this->j);
}

// 在文件流中查找模式串
int Kmp::streamSearching(int i, int j)
{
	pfin->seekg(offset + j);

	char c;
	while (j < fileLen && !pfin->eof())
	{
		pfin->read(&c, sizeof c);
		while (i > -1 && matchStr[i] != c)
			i = kmpNext[i];
		i++;
		j++;
		if (i >= matchLen)
		{
			this->i = kmpNext[i];
			this->j = j;
			return j - i;
		}
	}
	return -1;
}

int Kmp::frun(std::ifstream* m_fin, char* match, int m_matchLen, int m_offset, int m_fileLen)
{
	pfin = m_fin;
	fileLen = m_fileLen;
	offset = m_offset;

	/* Preprocessing */
	generateNext(match, m_matchLen);

	/* Searching */
	int origalPos = pfin->tellg();
	int index = offset + streamSearching(0, 0);
	pfin->seekg(origalPos);
	return index;
}

int Kmp::frun(std::ifstream* m_fin, int m_offset, int m_fileLen)
{
	if (!(kmpNext && matchStr))
		return -1;

	pfin = m_fin;
	fileLen = m_fileLen;
	offset = m_offset;

	int origalPos = pfin->tellg();
	int index = offset + streamSearching(0, 0);
	pfin->seekg(origalPos);
	return index;
}

int Kmp::frun()
{
	if (!(kmpNext && matchStr && pfin))
		return -1;

	int origalPos = pfin->tellg();
	int index = offset + streamSearching(this->i, this->j);
	pfin->seekg(origalPos);
	return index;
}

Kmp::Kmp(const Kmp& m_kmp)
{
	matchLen = m_kmp.matchLen;

	// 重新保存模式串
	matchStr = new char[matchLen];
	memcpy(matchStr, m_kmp.matchStr, matchLen);

	// 重新保存Next数组
	kmpNext = new int[matchLen + 1];
	memcpy(kmpNext, m_kmp.kmpNext, (matchLen + 1) * sizeof(int));
}
