//  [6/30/2014 %fengbangyao@gmail.com%]

#ifndef __PROCESS_H__
#define __PROCESS_H__
#include <TlHelp32.h>
#include <Psapi.h>
#include <string>


#pragma comment(lib,"psapi.lib")
namespace winult
{
	typedef std::basic_string<TCHAR> generic_string;
	typedef DWORD PID;
	class Process
	{
	public:
		Process():_pid(0),_error(0),_handle(NULL),_thread(NULL){};
		~Process(){Reset();};

		BOOL Create(generic_string sWorkDir, generic_string sExeName, generic_string sCmdLine, BOOL bShow);
		BOOL Wait(int maxWaitTime);
		PID  Find(generic_string sName);
		BOOL Kill(int maxWaitTime);
		BOOL DebugPrivilege();
		HANDLE  CurrentProcess();
		PID     CurrentProcessId();

		HANDLE GetHandle(){return _handle;}
		PID    GetProcessId(){return _pid;}
		int GetErrorCode(){ return _error;}
		generic_string HostFileName();	
		void Reset();
	private:
		PID _pid;
		HANDLE _thread;
		HANDLE _handle;
		int _error;
	};

	void Process::Reset()
	{
		_pid   = 0;
		_error = 0;
		if (_thread != NULL)
		{
			CloseHandle(_thread);
			_thread = NULL;
		}

		if (_handle != NULL)
		{
			CloseHandle(_handle);
			_handle = NULL;
		}
	}


	HANDLE  Process::CurrentProcess()
	{
		Reset();
		_handle = GetCurrentProcess();
		return _handle;
	}

	PID Process::CurrentProcessId()
	{
		Reset();
		_pid = GetCurrentProcessId();
		return _pid;
	}

	BOOL Process::Create(generic_string sWorkDir, generic_string sExeName, generic_string sCmdLine, BOOL bShow)
	{
		Reset();
		STARTUPINFOW startupInfo = {0};
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.dwFlags = STARTF_USESHOWWINDOW;
		startupInfo.wShowWindow = bShow;
		PROCESS_INFORMATION pi = {0};

		if ((sWorkDir.at(sWorkDir.length() - 1) != '\\') && (sWorkDir.at(sWorkDir.length() - 1) != '/'))
		{
			sWorkDir.append(L"\\");
		}
		generic_string sAppName = sWorkDir + sExeName;
		BOOL bResult = CreateProcess(sAppName.data(), const_cast<TCHAR*>(sCmdLine.data()),
									NULL, NULL, NULL,CREATE_FORCEDOS, NULL,sWorkDir.data(),&startupInfo, &pi);
		if (bResult == TRUE)
		{
			_thread = pi.hThread;
			_handle = pi.hProcess;
			_pid    = pi.dwProcessId;
		}
		else
		{
			_error = GetLastError();
		}

		return bResult;
	}

	PID Process::Find(generic_string sName)
	{
		Reset();
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		if (hSnapShot == NULL) return 0;


		PROCESSENTRY32 stProcessEntry = {0};
		stProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnapShot,&stProcessEntry);

		do 
		{
			if(_tcscmp(stProcessEntry.szExeFile,sName.data()) == 0)
			{
				HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, stProcessEntry.th32ProcessID );
				if (hProcess == NULL) return 0;

				_pid    = stProcessEntry.th32ProcessID;
				_handle = hProcess;
				break;

			}
		} while (Process32Next(hSnapShot,&stProcessEntry));

		CloseHandle(hSnapShot);
		return _pid;
	}

	BOOL Process::Wait(int maxWaitTime)
	{
		if (_pid > 0 && _handle != NULL)
		{
			return WAIT_OBJECT_0 == WaitForSingleObject(_handle, maxWaitTime);
		}
		
		return FALSE;
	}

	BOOL Process::Kill(int maxWaitTime)
	{
		BOOL bRet = TerminateProcess(_handle, 0);
		if (maxWaitTime <= 0) return bRet;

		
		return WAIT_OBJECT_0 == WaitForSingleObject(_handle, maxWaitTime);
	}

	generic_string Process::HostFileName()
	{
		TCHAR szPath[MAX_PATH] = {0};
		int bSize = GetModuleFileNameEx(_handle, NULL,szPath, MAX_PATH);
		return generic_string(szPath, bSize);
	}

	BOOL Process::DebugPrivilege()
	{
		if (_handle) CurrentProcess();
		HANDLE token = NULL;
		if (!OpenProcessToken(_handle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
			return FALSE;

		LUID luid;
		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
			return FALSE;

		TOKEN_PRIVILEGES pToken;
		pToken.PrivilegeCount = 1;
		pToken.Privileges[0].Luid = luid;
		pToken.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (!AdjustTokenPrivileges(token, FALSE, &pToken, NULL, NULL, NULL))
			return FALSE;

		return TRUE;
	}

	
};
#endif//__PROCESS_H__