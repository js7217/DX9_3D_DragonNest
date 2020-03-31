#ifndef Timer_h__
#define Timer_h__

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer : public CBase
{
private:
	explicit CTimer(void);
	virtual ~CTimer(void);

public:
	_float		Get_TimeDelta(void) const { return m_fTimeDelta; }
	void		SetUp_TimeDelta(void);

public:
	HRESULT		Ready_Timer(void);

private:
	LARGE_INTEGER		m_FrameTime;
	LARGE_INTEGER		m_FixTime;
	LARGE_INTEGER		m_LastTime;
	LARGE_INTEGER		m_CpuTick;

	_float				m_fTimeDelta;


public:
	static CTimer*		Create(void);
	virtual void		Free(void);
};

END

#endif // Timer_h__
