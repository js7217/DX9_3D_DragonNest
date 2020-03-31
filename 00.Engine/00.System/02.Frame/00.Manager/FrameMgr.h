#ifndef FrameMgr_h__
#define FrameMgr_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Frame.h"

BEGIN(Engine)

class ENGINE_DLL CFrameMgr : public CBase 
{
	DECLARE_SINGLETON(CFrameMgr)

private:
	explicit CFrameMgr(void);
	virtual ~CFrameMgr(void);

public:
	_bool	IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);
public:
	HRESULT	Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit);

private:
	CFrame*		Find_Frame(const _tchar* pFrameTag);

private:
	map<const _tchar*, CFrame*>			m_mapFrame;

public:
	virtual void		Free(void);

};

END
#endif // FrameMgr_h__
