//  [7/2/2014 %fengbangyao@gmail.com%]

#ifndef __FILE_VERSION_H__
#define __FILE_VERSION_H__

#include <WinVer.h>
#include <string>
#include <sstream>
#include <iomanip>
#pragma comment(lib,"version.lib")


/*
    refer:
	use tool(resource hacker) open PE File.
	to view "VersionInfo" item.
*/
namespace winult
{
	typedef std::basic_string<TCHAR>        generic_string;
	typedef std::basic_ostringstream<TCHAR> generic_stream;
	class FileVersion
	{
	public:
		FileVersion();
		~FileVersion();

		BOOL Open(generic_string sFilePath);
		VOID Close();

		generic_string GetValue(generic_string sKey, DWORD dwLangCharset = 0);
		generic_string GetFixedFileVersion();
		generic_string GetFixedProductVersion();
		
		generic_string GetFileDescription() {return GetValue(L"FileDescription"); }
		generic_string GetFileVersion()     {return GetValue(L"FileVersion");     }
		generic_string GetInternalName()    {return GetValue(L"InternalName");    }
		generic_string GetCompanyName()     {return GetValue(L"CompanyName");     }
		generic_string GetLegalCopyright()  {return GetValue(L"LegalCopyright");  }
		generic_string GetOriginalFilename(){return GetValue(L"OriginalFilename");}
		generic_string GetProductName()     {return GetValue(L"ProductName");     }
		generic_string GetProductVersion()  {return GetValue(L"ProductVersion");  }


		//Operations
	private:
		BOOL GetFixedInfo(VS_FIXEDFILEINFO& vsffi);
	private:
		generic_string m_sFilePath;
		PBYTE m_pVersionData;
		DWORD m_dwCharset;
	};

	FileVersion::FileVersion()
		:m_sFilePath(L"")
		,m_pVersionData(NULL)
		,m_dwCharset(0)
	{}
	FileVersion::~FileVersion()
	{
		Close();
	}

	BOOL FileVersion::Open(generic_string sFilePath)
	{
		Close();
		if (!PathFileExists(sFilePath.data())) return FALSE;
		m_sFilePath = sFilePath;
		
		DWORD dwHandle = 0;
		DWORD dwSize = GetFileVersionInfoSize(m_sFilePath.data(), &dwHandle);
		if (dwSize > 0)
		{
			m_pVersionData = new BYTE[dwSize];
			ZeroMemory(m_pVersionData, dwSize * sizeof(BYTE));
			if(!GetFileVersionInfo(m_sFilePath.data(), dwHandle, dwSize, (LPVOID)m_pVersionData))
			{
				Close();
				return FALSE;
			}
			
			UINT nLen = 0;
			DWORD* pTransTable = 0;
			if(!VerQueryValue(m_pVersionData, L"\\VarFileInfo\\Translation", (LPVOID*)&pTransTable, &nLen))
			{
				int err = GetLastError();
				Close();
				return FALSE;
			}

			m_dwCharset =  MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

			return TRUE;
		}

	}

	VOID FileVersion::Close()
	{
		m_dwCharset = 0;
		m_sFilePath.clear();
		if (m_pVersionData != NULL)
		{
			delete m_pVersionData;
			m_pVersionData = NULL;
		}
	}

	generic_string FileVersion::GetValue(generic_string sKey, DWORD dwLangCharset /* = 0 */)
	{
		if (!sKey.empty() && (m_pVersionData != NULL))
		{
			if (dwLangCharset == 0) dwLangCharset = m_dwCharset;
			
			UINT nSize = 0;
			LPVOID lpData = NULL;
			
			generic_stream ostr;
			ostr<<std::setbase(16)<<std::setfill(L'0')<<std::setw(8)<<dwLangCharset;
			generic_string sCharset = ostr.str();
			
			generic_string sBlockName(L"\\StringFileInfo\\");
			sBlockName.append(sCharset);
			sBlockName.append(L"\\").append(sKey);
			if (VerQueryValue((LPCVOID)m_pVersionData, sBlockName.data(), (LPVOID*)&lpData, &nSize))
			{
				return generic_string((TCHAR*)lpData, nSize);
			}
		}
		return generic_string();
	}

	BOOL FileVersion::GetFixedInfo(VS_FIXEDFILEINFO& vsffi)
	{
		if (m_pVersionData == NULL) return FALSE;
		
		UINT nSzie = 0;
		VS_FIXEDFILEINFO* pVsffi;
		if (VerQueryValue((LPCVOID)m_pVersionData, L"\\", (LPVOID*)&pVsffi, &nSzie))
		{
			vsffi = *pVsffi;
			return TRUE;
		}

		return FALSE;
	}

	generic_string FileVersion::GetFixedFileVersion()
	{
		VS_FIXEDFILEINFO vsffi = {0};

		if (GetFixedInfo(vsffi))
		{
			generic_stream oss;
			oss<<HIWORD(vsffi.dwFileVersionMS)<<L","<<LOWORD(vsffi.dwFileVersionMS)<<L","
			   <<HIWORD(vsffi.dwFileVersionLS)<<L","<<LOWORD(vsffi.dwFileVersionLS);
			return oss.str();
		}

		return generic_string();
	}

	generic_string FileVersion::GetFixedProductVersion()
	{
		generic_string sVersion(L"");
		VS_FIXEDFILEINFO vsffi = {0};
		if (GetFixedInfo(vsffi))
		{
			generic_stream oss;
			oss<<HIWORD(vsffi.dwProductVersionMS)<<L","<<LOWORD(vsffi.dwProductVersionMS)<<","
			   <<HIWORD(vsffi.dwProductVersionLS)<<L","<<LOWORD(vsffi.dwProductVersionLS);
			return oss.str();
		}
		
		return generic_string();
	}
}
#endif//__FILE_VERSION_H__