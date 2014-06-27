
//  [6/25/2014 %fengbangyao@gmail.com%]
#ifndef __SHARE_LIBRARY_H
#define __SHARE_LIBRARY_H


#include <string>
typedef std::basic_string<TCHAR> generic_string;
class ShareLibrary
{
public:
	ShareLibrary():m_hModule(NULL){};
	~ShareLibrary(){};

	BOOL Load(generic_string sPath);
	BOOL Isload(){return m_hModule != NULL;}
	BOOL Unload();

	int   GetErrorNo(){ return _errorno;}
	PVOID GetSymbol(std::string sMothodName);

private:
	HMODULE m_hModule;
	int     _errorno;
	generic_string m_sPath;
};


inline BOOL ShareLibrary::Load(generic_string sPath)
{
	m_sPath = sPath;
	m_hModule = LoadLibrary(sPath.data());
	_errorno = (m_hModule == NULL) ? GetLastError() : 0;

	return m_hModule != NULL;
}

inline BOOL ShareLibrary::Unload()
{
	if (Isload())
	{
		if (!FreeLibrary(m_hModule))
		{
			_errorno = GetLastError();
			return FALSE;
		}
	}
	m_hModule = NULL;
	m_sPath.clear();
	
	return TRUE;
}

inline PVOID ShareLibrary::GetSymbol(std::string sMethodName)
{
	PVOID pFunc = NULL;
	if(m_hModule == NULL) return pFunc;
	pFunc = (PVOID)GetProcAddress(m_hModule, sMethodName.data());
	return pFunc;
}



#endif//__SHARE_LIBRARY_H