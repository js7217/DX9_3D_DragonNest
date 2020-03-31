#ifndef TimeMgr_h__
#define TimeMgr_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

class ENGINE_DLL CTimeMgr : public CBase
{
	DECLARE_SINGLETON(CTimeMgr)

private:
	explicit CTimeMgr(void);
	virtual ~CTimeMgr(void);

public:
	_float		Get_TimeDelta(const _tchar* pTimerTag);
	void		SetUp_TimeDelta(const _tchar* pTimerTag);

public:
	HRESULT	Ready_Timer(const _tchar* pTimerTag);

private:
	CTimer*	Find_Timer(const _tchar* pTimerTag);

private:
	map<const _tchar*, CTimer*>			m_mapTimer;

public:
	virtual void Free(void);
};

END
#endif // TimeMgr_h__
