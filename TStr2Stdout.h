//  [6/27/2014 %fengbangyao@gmail.com%]

#ifndef __TSTR_2_STDOUT_H__
#define __TSTR_2_STDOUT_H__
#include <iostream>
#include <assert.h>
namespace winult
{
	template <class T>
	class TStr2Stdout
	{
	public:
		TStr2Stdout(const T* pStr):m_pStr(pStr){};

		template <class T>
		friend std::ostream& operator<<(std::ostream& os, TStr2Stdout& pStr);
	private:
		const T* m_pStr;
	};

	template<class T>
	std::ostream& operator<<(std::ostream& os, TStr2Stdout<T>& pStr)
	{
		switch (sizeof(T))
		{
		case 1:
			os<<pStr.m_pStr;
			break;
		case 2: 
			wprintf(L"%s",pStr.m_pStr);
			break;
		default:
			assert(0);
			break;
		}
		return os;
	}

};
#endif//__TSTR_2_STDOUT_H__