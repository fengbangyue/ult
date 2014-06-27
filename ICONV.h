//  [6/26/2014 %fengbangyao@gmail.com%]

#ifndef __ICONV_H__
#define __ICONV_H__

#include <string>
class ICONV
{
public:
	static std::wstring ANSI2UCS2(const std::string&  str);
	static std::string  UCS22ANSI(const std::wstring& str);
	static std::wstring UTF82UCS2(const std::string&  str);
	static std::string  UCS22UTF8(const std::wstring& str);
	static std::string  ASNI2UTF8(const std::string&  str);
	static std::string  UTF82ANSI(const std::string&  str);
};


inline std::wstring ICONV::ANSI2UCS2(const std::string& str)
{
	unsigned int lenUCS2 = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pUCS2 = new wchar_t[lenUCS2 + 1];
	memset(pUCS2, 0, (lenUCS2 + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pUCS2, lenUCS2);

	std::wstring sUCS2(pUCS2, lenUCS2);
	delete pUCS2;
	pUCS2 = NULL;
	return sUCS2;
}

inline std::string ICONV::UCS22ANSI(const std::wstring& str)
{
	unsigned int lenAscii = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
	char* pAscii = new char[lenAscii + 1];
	memset(pAscii, 0, (lenAscii + 1) * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.length(), pAscii, lenAscii, NULL,NULL);

	std::string sAscii(pAscii, lenAscii);
	delete pAscii;
	pAscii = NULL;
	return sAscii;
}


inline std::wstring ICONV::UTF82UCS2(const std::string& str)
{
	unsigned int lenUCS2 = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pUCS2 = new wchar_t[lenUCS2 + 1];
	memset(pUCS2, 0, (lenUCS2 + 1) * sizeof(wchar_t));

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pUCS2, lenUCS2);
	std::wstring sUCS2(pUCS2, lenUCS2);
	delete pUCS2;
	pUCS2 = NULL;

	return sUCS2;
}

inline std::string  ICONV::UCS22UTF8(const std::wstring& str)
{
	unsigned int lenUTF8 = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
	char* pUTF8 = new char[lenUTF8 + 1];
	memset(pUTF8, 0, (lenUTF8 + 1) * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), pUTF8, lenUTF8, NULL,NULL);
	std::string sUTF8(pUTF8, lenUTF8);
	delete pUTF8;
	pUTF8 = NULL;

	return sUTF8;
}

inline std::string  ICONV::ASNI2UTF8(const std::string&  str)
{
	return UCS22UTF8(ANSI2UCS2(str));
}

inline std::string  ICONV::UTF82ANSI(const std::string&  str)
{
	return UCS22ANSI(UTF82UCS2(str));
}


#endif//__ICONV_H__
