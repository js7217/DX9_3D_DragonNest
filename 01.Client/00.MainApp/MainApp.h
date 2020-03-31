#ifndef MainApp_h__
#define MainApp_h__

#include "Defines.h"
#include "Base.h"

#include "Logo.h"

namespace Engine
{
	class CGraphicDev;
	class CManagement;
	class CKeyMgr;
	class CRenderer;
}

class CMainApp : public CBase
{
	// 积己磊, 家戈磊
private:
	explicit CMainApp(void);
	virtual ~CMainApp(void);

	// Get
	// Set
	// Public
public:
	HRESULT		Ready_MainApp(void);
	_int		Update_MainApp(_float fTimeDelta);
	void		Render_MainApp(void);

private:
	HRESULT		SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT		Ready_Resource(LPDIRECT3DDEVICE9 pGraphicDev, RESOURCEID eID);
	HRESULT		Ready_OpeningScene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagementInstance);
	HRESULT		Ready_Component_Prototype();
private:
	Engine::CGraphicDev*	m_pDeviceInstance = nullptr;
	Engine::CManagement*	m_pManagementInstance = nullptr;
	LPDIRECT3DDEVICE9		m_pGrpahicDev;
	_tchar					m_szFPS[128];
	_float					m_fTime = 0.f;
	_ulong					m_dwRenderCnt = 0;
	

	Engine::CKeyMgr*		m_pKeyMgr = nullptr;
	Engine::CRenderer*		m_pRenderer = nullptr;
	// 积己
public:
	static CMainApp*		Create(void);
	// 秦力
private:
	virtual void			Free(void);

};
#endif // MainApp_h__
