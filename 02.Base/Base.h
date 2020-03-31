#ifndef Base_h__
#define Base_h__

class _declspec(dllexport)	CBase
{
protected:
	inline explicit CBase(void);
	inline virtual ~CBase(void);

public:
	inline unsigned long AddRef(void);
	inline unsigned long Release(void);

private:
	unsigned long		m_dwRefCnt;
public:
	inline virtual void Free(void) = 0;

};


#include "Base.inl"

#endif // Base_h__
