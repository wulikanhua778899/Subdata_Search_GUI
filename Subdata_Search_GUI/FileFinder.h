#pragma once
#include <afx.h>
#include <vector>
class FileFinder
{
public:
    BOOL GetFiles(CString path, std::vector<CString>& files);
};
