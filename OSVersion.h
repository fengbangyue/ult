//  [6/30/2014 %fengbangyao@gmail.com%]

#ifndef __OS_VERSION_H__
#define __OS_VERSION_H__
#include <assert.h>
namespace winult{

	typedef std::basic_string<TCHAR> generic_string;
	class OSVersion
	{
		enum winVer{WV_UNKNOWN, WV_WIN32S, WV_95, WV_98, WV_ME, WV_NT, WV_W2K, WV_XP, WV_S2003, WV_XPX64, WV_VISTA, WV_WIN7, WV_WIN8, WV_WIN81};
		typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
	public:
		OSVersion(){};
		~OSVersion(){};

		BOOL IsWin64();
		int GetWindowsVersion();
		generic_string GetVersionText(int nVersion);		
	};

	BOOL OSVersion::IsWin64()
	{
		typedef BOOL (WINAPI *lpfn_IsWOW64Process)(HANDLE, PBOOL);

#if define(WIN64)
		return TRUE;
#endif
		BOOL bIsWow64 = FALSE;
		lpfn_IsWOW64Process fnIsWow64Process = (lpfn_IsWOW64Process)GetProcAddress(
			GetModuleHandle(L"kernel32"), "IsWow64Process");
		if (NULL != fnIsWow64Process)
		{
			if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
			{
				assert(0);//
			}
		}

		return bIsWow64;

	}
	int OSVersion::GetWindowsVersion()
	{
		OSVERSIONINFOEX osvi;
		SYSTEM_INFO si;
		PGNSI pGNSI;

		ZeroMemory(&si, sizeof(SYSTEM_INFO));
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		BOOL bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *)&osvi);
		if (!bOsVersionInfoEx)
		{
			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
				return WV_UNKNOWN;
		}

		pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
		if(pGNSI != NULL)
			pGNSI(&si);
		else
			GetSystemInfo(&si);

		switch (osvi.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
			{
				if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3 )
				{
					return WV_WIN81;
				}

				if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 )
				{
					return WV_WIN8;
				}

				if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 )
				{
					return WV_WIN7;
				}

				if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
				{
					return WV_VISTA;
				}

				if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
				{
					if (osvi.wProductType == VER_NT_WORKSTATION &&
						si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
					{
						return WV_XPX64;
					}
					return WV_S2003;
				}

				if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
					return WV_XP;

				if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
					return WV_W2K;

				if ( osvi.dwMajorVersion <= 4 )
					return WV_NT;
			}
			break;

			// Test for the Windows Me/98/95.
		case VER_PLATFORM_WIN32_WINDOWS:
			{
				if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
				{
					return WV_95;
				} 

				if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
				{
					return WV_98;
				} 

				if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
				{
					return WV_ME;
				}
			}
			break;

		case VER_PLATFORM_WIN32s:
			return WV_WIN32S;

		default :
			return WV_UNKNOWN;
		}
		return WV_UNKNOWN; 
	}

	generic_string OSVersion::GetVersionText(int nVersion)
	{
		generic_string sVersionName(L"WV_UNKNOWN");
		generic_string verText[] = {generic_string(L"WV_UNKNOWN"),generic_string(L"WV_WIN32S"),
			generic_string(L"WV_95"),generic_string(L"WV_98"),generic_string(L"WV_ME"),generic_string(L"WV_NT"),generic_string(L"WV_W2K"),
			generic_string(L"WV_XP"),generic_string(L"WV_S2003"),generic_string(L"WV_XPX64"),generic_string(L"WV_VISTA"),
			generic_string(L"WV_WIN7"),generic_string(L"WV_WIN8"),generic_string(L"WV_WIN81")};

		int nIndex = (sizeof(verText) / ( sizeof(generic_string) ) );
		if (nVersion < 0 || (nVersion >=  nIndex) )
		{
			nVersion = 0;
		}
		sVersionName = verText[nVersion];
		return sVersionName;
	}
};

#endif//__OS_VERSION_H__