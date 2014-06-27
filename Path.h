//  [6/25/2014 %fengbangyao@gmail.com%]

#ifndef __PATH_H__
#define __PATH_H__

#include <string>
#include <Shlwapi.h>
#include <ShlObj.h>
#include <ImageHlp.h>
#include "ICONV.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "imagehlp.lib")
#pragma comment(lib, "Shell32.lib")

typedef std::basic_string<TCHAR> generic_string;
namespace winult
{
	class Path
	{
	public:
		Path(){};
		~Path(){};

		void ToStdout(generic_string s);
		generic_string GetCurrentProcessPath();
		generic_string GetLastField(generic_string sFileName);
		generic_string GetParent(generic_string sPath);
		generic_string GetDriver(generic_string sPath);
		generic_string RemoveBackslash(generic_string sPath);

		generic_string RelativeToAbsolute(generic_string sBase,generic_string sRelative);

		generic_string GetTempDirectory();
		generic_string GetSpecialFolder(int csIDL);
		BOOL IsDiectoryFormat(generic_string sPath);
		BOOL IsDirectory(generic_string sPath);
		BOOL MakeSureDirExists(generic_string sPath); 
		BOOL IsPathExists(generic_string sPath);
		BOOL DeleteNoEmptyDir(generic_string sPath);
	};

	inline void Path::ToStdout(generic_string s)
	{
		switch(sizeof(TCHAR))
		{
		case 1:
			std::cout<<s.data()<<std::endl;;
			break;
		case 2:
			wprintf(L"%s\n", s.data());
			break;
		}
	}

	inline generic_string Path::GetCurrentProcessPath()
	{
		CBuffer<TCHAR> buff(MAX_PATH);
		GetModuleFileName(NULL, buff, MAX_PATH);
		return generic_string(buff);
	}

	inline generic_string Path::GetLastField(generic_string sFileName)
	{
		generic_string::size_type Pos;
		Pos = ((Pos = sFileName.rfind('\\')) == generic_string::npos) ?
			sFileName.rfind('/') : Pos;

		if (Pos != generic_string::npos)
		{
			return sFileName.substr(Pos + 1);
		}

		return generic_string();
	}

	inline generic_string Path::GetParent(generic_string sPath)
	{
		generic_string::size_type Pos;
		Pos = ((Pos = sPath.rfind('\\')) == generic_string::npos) ?
			sPath.rfind('/') : Pos;
		if (Pos != generic_string::npos)
		{
			return sPath.substr(0, Pos);
		}
		return generic_string();
	}

	inline generic_string Path::GetDriver(generic_string sPath)
	{
		if (sPath.empty())
		{
			return generic_string();
		}
		return sPath.substr(0, 1);
	}

	inline generic_string Path::RemoveBackslash(generic_string sPath)
	{
		generic_string::size_type pos = sPath.length();
		for (generic_string::reverse_iterator iter = sPath.rbegin(); iter != sPath.rend(); ++iter)
		{
			if ((*iter != '\\') && (*iter != '/')) break;
			pos--;
		}		

		return sPath.substr(0,pos);
	}

	inline generic_string Path::RelativeToAbsolute(generic_string sBase, generic_string sRelative)
	{
		TCHAR szPath[MAX_PATH] = {0};
		memcpy(szPath, sBase.data(), sBase.length() * sizeof(TCHAR));

		PathAppend(szPath, sRelative.data());

		return generic_string(szPath);
	}

	inline BOOL Path::MakeSureDirExists(generic_string sPath)
	{
		if (sPath.rfind('\\') != sPath.length() - 1)
		{
			sPath.append(generic_string(L"\\"));
		}

		std::string sDirPath("");
		if (sizeof(sPath.at(0)) == sizeof(wchar_t))
		{
			sDirPath = ICONV::UCS22ANSI(sPath);
		}
		else
		{
			sDirPath = *(std::string*)&sPath;
		}
		return MakeSureDirectoryPathExists(sDirPath.data());
	}

	inline BOOL Path::IsPathExists(generic_string sPath)
	{

		return PathFileExists(sPath.data());
	}

	inline BOOL Path::IsDiectoryFormat(generic_string sPath)
	{
		if (sPath.empty()) return FALSE;
		if (sPath.at(sPath.length() - 1) == '\\') return TRUE;
		if (sPath.at(sPath.length() - 1) == '/')  return TRUE;

		return FALSE;
	}

	inline BOOL Path::IsDirectory(generic_string sPath)
	{
		if (IsDiectoryFormat(sPath))
		{
			sPath.append(L"*.*");
		}
		else
		{
			sPath.append(L"\\*.*");
		}
		WIN32_FIND_DATA dataFind;
		memset(&dataFind, 0, sizeof(WIN32_FIND_DATA));
		HANDLE hFile = FindFirstFile(sPath.data(), &dataFind);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			FindClose(hFile);
			return FALSE;
		}

		FindClose(hFile);
		return TRUE;
	}

	BOOL  Path::DeleteNoEmptyDir(generic_string sPath)
	{
		generic_string sSubPath(sPath);
		if (IsDiectoryFormat(sSubPath))
		{
			sSubPath.append(L"*.*");
		}
		else
		{
			sSubPath.append(L"\\*.*");
		}

		WIN32_FIND_DATA dataFind;
		memset(&dataFind, 0, sizeof(WIN32_FIND_DATA));
		HANDLE hFile = FindFirstFile(sSubPath.data(), &dataFind);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			FindClose(hFile);
			return TRUE;
		}

		BOOL bLoop = TRUE;
		while (bLoop)
		{
			if (lstrcmp(dataFind.cFileName, L".") && lstrcmp(dataFind.cFileName, L".."))
			{
				generic_string sNextPath(sPath);
				sNextPath.append(L"\\").append(dataFind.cFileName);
				if (IsDirectory(sNextPath))
				{
					if(!DeleteNoEmptyDir(sNextPath)) return FALSE;
				}
				else
				{
					if(!DeleteFile(sNextPath.data())) return FALSE;
				}
			}
			memset(&dataFind, 0, sizeof(WIN32_FIND_DATA));
			bLoop = FindNextFile(hFile, &dataFind);
		}

		FindClose(hFile);
		if (!RemoveDirectory(sPath.data())) return FALSE;		

		return TRUE;
	}

	generic_string  Path::GetTempDirectory()
	{
		TCHAR szTemp[MAX_PATH] = {0};
		DWORD dwLength = GetTempPath(MAX_PATH, szTemp);

		return generic_string(szTemp,dwLength);
	}

	generic_string Path::GetSpecialFolder(int csIDL)
	{
		ITEMIDLIST *pidl;
		SHGetSpecialFolderLocation(NULL, csIDL, &pidl);
		TCHAR szFolder[MAX_PATH];
		SHGetPathFromIDList(pidl, szFolder);

		return generic_string(szFolder);
	}
};



#endif//__PATH_H__