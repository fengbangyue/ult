
#ifndef BY_BUFFER_CHAR_WCHAR_T
#define BY_BUFFER_CHAR_WCHAR_T
//  [6/23/2014 %fengbangyao@gmail.com%]


//使用CBuffer对char或wchar_t进行管理
//解决和Windows API交互时使用缓冲区问题
//不要试图对CBuffer_T添加Append,SubString类似的函数
//如果需要可以直接使用stl东西
template <class C>
class CBuffer_T
{
public:

	CBuffer_T(int nSize);
	CBuffer_T(C* sBuf, int nSize);
	~CBuffer_T(void);

	operator C* (){return _buffer;}

	void Empty();
	int Length(){return m_nSize;}

private:
	C* _buffer;
	int m_nSize;
};

template <class C>
inline CBuffer_T<C>::CBuffer_T(int nSize)
:m_nSize(nSize)
{
	_buffer = new C[nSize];
	Empty();
}

template <class C>
inline CBuffer_T<C>::CBuffer_T(C* sBuf, int nSize)
{
	m_nSize = nSize;
	_buffer = new C[m_nSize];
	memcpy(_buffer, sBuf, m_nSize);
}

template <class C>
inline CBuffer_T<C>::~CBuffer_T(void)
{
	m_nSize = 0;
	delete []_buffer;
	_buffer = NULL;
}


template <class C>
inline void  CBuffer_T<C>::Empty() 
{
	memset(_buffer, 0, m_nSize * sizeof(C));
}

#endif //BY_BUFFER_CHAR_WCHAR_T