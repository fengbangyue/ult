//  [7/1/2014 %fengbangyao@gmail.com%]

#ifndef __REGEDIT_H__
#define __REGEDIT_H__

#include <string>
namespace winult
{
	typedef std::basic_string<TCHAR> generic_string;
	class Regedit
	{
	public:
		Regedit();
		~Regedit();

		BOOL Create(HKEY hKey, generic_string sSubKey, REGSAM samAccess = KEY_ALL_ACCESS);
		BOOL Open(HKEY hKey, generic_string sSubKey, REGSAM samAccess = KEY_ALL_ACCESS);
		BOOL Delete(HKEY hKey, generic_string sSubKey);
		BOOL Close();

		/*
		    MSDN:
			Calling RegFlushKey is an expensive operation that significantly affects system-wide performance as 
			it consumes disk bandwidth and blocks modifications to all keys by all processes in the registry hive 
			that is being flushed until the flush operation completes. RegFlushKey should only be called explicitly 
			when an application must guarantee that registry changes are persisted to disk immediately after modification. 
			All modifications made to keys are visible to other processes without the need to flush them to disk.
		*/
		BOOL FlushKey();

		BOOL ReadDWORD(const generic_string& sName, DWORD& dwValue);
		BOOL ReadString(const generic_string& sName, generic_string& sValue);
		BOOL SetDWORD(const generic_string& sName, const DWORD& dwValue);
		BOOL SetString(const generic_string& sName, const generic_string& sValue);

		generic_string GetErrorMessage();

	private:
		HKEY _hKey;
		HKEY _hSubKey;
		DWORD _error;
	};

	Regedit::Regedit()
		:_hKey(NULL)
		,_hSubKey(NULL)
		,_error(0)
	{}

	Regedit::~Regedit()
	{
		Close();
	}

	generic_string Regedit::GetErrorMessage()
	{
		TCHAR szBuffer[MAX_PATH] = {0};
		DWORD dwLen = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM ,NULL,_error, 0,szBuffer, MAX_PATH, NULL);
		return generic_string(szBuffer, dwLen);
	}
	BOOL Regedit::Create(HKEY hKey, generic_string sSubKey, REGSAM samAccess /* = KEY_ALL_ACCESS */)
	{
		_error = RegCreateKeyEx(hKey, sSubKey.data(), 0, NULL, REG_OPTION_NON_VOLATILE,
			samAccess, NULL, &_hSubKey, NULL);
		return ERROR_SUCCESS == _error;
	}

	BOOL Regedit::Open(HKEY hKey, generic_string sSubKey, REGSAM samAccess /* = KEY_ALL_ACCESS */)
	{
		_error = RegOpenKeyEx(hKey, sSubKey.data(), 0, samAccess, &_hSubKey);
		return ERROR_SUCCESS == _error;
	}

	BOOL Regedit::FlushKey()
	{
		_error = RegFlushKey(_hKey);
		return ERROR_SUCCESS == _error;
	}

	BOOL Regedit::Delete(HKEY hKey, generic_string sSubKey)
	{
		_error =  RegDeleteKeyEx(hKey, sSubKey.data(), KEY_WOW64_32KEY, 0);
		return ERROR_SUCCESS == _error;
	}

	BOOL Regedit::ReadDWORD(const generic_string& sName, DWORD& dwValue)
	{
		DWORD dwType = 0;
		DWORD dwSize = sizeof(DWORD);

		_error = RegQueryValueEx(_hSubKey, sName.data(), NULL, &dwType, (LPBYTE)&dwValue, &dwSize);
		return ERROR_SUCCESS == _error;
	}

	BOOL Regedit::ReadString(const generic_string& sName, generic_string& sValue)
	{
		DWORD dwType = 0;
		TCHAR szBuffer[MAX_PATH] = {0};
		DWORD dwSize = sizeof(szBuffer);

		_error = RegQueryValueEx(_hSubKey, sName.data(), NULL,  &dwType, (LPBYTE)szBuffer, &dwSize);
		if (ERROR_SUCCESS == _error)
		{
			sValue.append(szBuffer, dwSize);
		}
		return ERROR_SUCCESS == _error;
	}

	BOOL Regedit::SetDWORD(const generic_string& sName, const DWORD& dwValue)
	{
		_error = RegSetValueEx(_hSubKey, sName.data(), 0, REG_DWORD, (const PBYTE)&dwValue, sizeof(DWORD));
		return ERROR_SUCCESS == _error;
	}

	BOOL Regedit::SetString(const generic_string& sName, const generic_string& sValue)
	{
		_error = RegSetValueEx(_hSubKey, sName.data(), 0, REG_SZ, (const PBYTE)sValue.data(), sizeof(TCHAR) * (sValue.length() + 1));
		return ERROR_SUCCESS == _error;
	}

	BOOL Regedit::Close()
	{
		_error = 0;
		if (_hKey != NULL)
		{
			RegCloseKey(_hKey);
			_hKey = NULL;
		}

		if (_hSubKey != NULL)
		{
			RegCloseKey(_hSubKey);
			_hSubKey = NULL;
		}

		return TRUE;
	}
}
#endif//__REGEDIT_H__