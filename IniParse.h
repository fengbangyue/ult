//  [6/26/2014 %fengbangyao@gmail.com%]

#ifndef __INI_PARSE_H__
#define __INI_PARSE_H__

typedef std::basic_string<TCHAR> generic_string;
class IniFile
{
public:
	IniFile();
	IniFile(generic_string);
	~IniFile();

	BOOL Load(generic_string sFileName);
	BOOL Isload(){ return m_sIniFile.empty() == FALSE;}

	int  GetInteger(generic_string sSection, generic_string sField);
	generic_string  GetString(generic_string  sSection, generic_string sField);

	BOOL SetInteger(generic_string sSection, generic_string sField, int nValue);
	BOOL SetString(generic_string  sSection, generic_string sField, generic_string sValue);

private:
	generic_string m_sIniFile;
};

IniFile::IniFile()
:m_sIniFile(L"")
{
}
IniFile::IniFile(generic_string sFileName)
:m_sIniFile(sFileName)
{
}
IniFile::~IniFile()
{
	m_sIniFile.clear();
};

BOOL IniFile::Load(generic_string sFileName)
{
	if (!PathFileExists(sFileName.data())) return FALSE;
	m_sIniFile = sFileName;
	return TRUE;
}

int IniFile::GetInteger(generic_string sSection, generic_string sField)
{
	return GetPrivateProfileInt(sSection.data(), sField.data(), -1, m_sIniFile.data());
}

generic_string IniFile::GetString(generic_string sSection, generic_string sField)
{
	TCHAR szValue[MAX_PATH] = {0};
	int nLen = GetPrivateProfileString(sSection.data(), sField.data(), L"", szValue, MAX_PATH, m_sIniFile.data());
	return generic_string(szValue, nLen);
}

BOOL IniFile::SetInteger(generic_string sSection, generic_string sField, int nValue)
{
	TCHAR szValue[20] = {0}; 
	_itot(nValue, szValue, 10);

	return WritePrivateProfileString(sSection.data(), sField.data(),szValue, m_sIniFile.data());
}

BOOL IniFile::SetString(generic_string sSection, generic_string sField, generic_string sValue)
{
	return WritePrivateProfileString(sSection.data(), sField.data(), sValue.data(), m_sIniFile.data());
}


#endif//__INI_PARSE_H__