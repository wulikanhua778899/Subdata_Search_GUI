#include "pch.h"
#include "FileFinder.h"

BOOL FileFinder::GetFiles(CString path, std::vector<CString>& files)
{
	CFileFind filefinder;

	path.Append(TEXT("\\*.*"));
	if (filefinder.FindFile(path))
	{
		while (filefinder.FindNextFile())
		{
			if (filefinder.IsDots()) continue;
			if (filefinder.IsDirectory())		
				GetFiles(filefinder.GetFilePath(), files);		
			else
				files.push_back(filefinder.GetFilePath());
		}
		if (filefinder.IsDots()) goto _close;
		if (filefinder.IsDirectory())
			GetFiles(filefinder.GetFilePath(), files);	
		else
			files.push_back(filefinder.GetFilePath());
	}
_close:
	filefinder.Close();	// πÿ±’≤È’“«Î«Û
	return TRUE;
}
