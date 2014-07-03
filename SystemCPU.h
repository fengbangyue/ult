//  [7/3/2014 %fengbangyao@gmail.com%]

#ifndef __SYSTEM_CPU_H__
#define __SYSTEM_CPU_H__

namespace winult
{
	class SystemCPU
	{
	public:
		SystemCPU();
		~SystemCPU();

		DWORD Numbers();

		BOOL SetTimeLine();
		int Usage();


		__int64 FileTime2Int64(const FILETIME& time);
		__int64 FileTimeSub(const FILETIME& Ft1, const FILETIME& Ft2);

	private:
		FILETIME m_KeernelFT;
		FILETIME m_UserFT;
		FILETIME m_IdleFT;
	};

	SystemCPU::SystemCPU()
	{}
	SystemCPU::~SystemCPU()
	{}


	DWORD SystemCPU::Numbers()
	{
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		return sysInfo.dwNumberOfProcessors;
	}

	__int64 SystemCPU::FileTime2Int64(const FILETIME& time)
	{
		return time.dwHighDateTime << 32 | time.dwLowDateTime;
	}

	__int64 SystemCPU::FileTimeSub(const FILETIME& BigFt, const FILETIME& SmallFt)
	{
		__int64 nBig = BigFt.dwHighDateTime << 32 | BigFt.dwLowDateTime;
		__int64 nSmall = SmallFt.dwHighDateTime << 32 | SmallFt.dwLowDateTime;

		return nBig - nSmall;
	}

	BOOL SystemCPU::SetTimeLine()
	{
		return GetSystemTimes(&m_IdleFT, &m_KeernelFT, &m_UserFT);
	}

	int SystemCPU::Usage()
	{
		FILETIME kernelTime;
		FILETIME userTime;
		FILETIME idleTime;

		if(GetSystemTimes(&idleTime, &kernelTime, &userTime))
		{
			__int64 nIdle   = FileTimeSub(idleTime, m_IdleFT);
			__int64 nKernel = FileTimeSub(kernelTime, m_KeernelFT);
			__int64 nUser   = FileTimeSub(userTime, m_UserFT);

			m_IdleFT    = idleTime;
			m_KeernelFT = kernelTime;
			m_UserFT    = userTime;
			return (nUser + nKernel - nIdle) * 100 / (nUser + nKernel);
		}
		return -1;
	}
}

#endif//__SYSTEM_CPU_H__