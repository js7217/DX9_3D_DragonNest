#ifndef Target_Manager_h__
#define Target_Manager_h__

#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CTarget;
class ENGINE_DLL CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	explicit CTarget_Manager();
	virtual ~CTarget_Manager();
public:
	HRESULT Add_Target(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pTargetTag, _uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
	HRESULT Begin_MRT(const _tchar* pMRTTag);
	HRESULT End_MRT(const _tchar* pMRTTag);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName, const _tchar* pTargetTag);

#ifdef _DEBUG
	HRESULT Ready_Debug_Buffer(const _tchar* pTargetTag, _float fStartX, _float fStartY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_Buffer(const _tchar* pMRTag);
#endif
private:
	map<const _tchar*, CTarget*>				m_Targets;
	typedef map<const _tchar*, CTarget*>		TARGETS;
private:
	map<const _tchar*, list<CTarget*>>			m_MRT;
	typedef map<const _tchar*, list<CTarget*>>	MRT;

private:
	CTarget*	Find_Target(const _tchar* pTargetTag);
	list<CTarget*>*	Find_MRT(const _tchar* pMRTTag);
public:
	virtual void Free();
};

END
#endif // Target_Manager_h__
