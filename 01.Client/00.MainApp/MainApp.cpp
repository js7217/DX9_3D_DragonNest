#include "stdafx.h"
#include "MainApp.h"

#include "Export_Function.h"
#include "Renderer.h"

CMainApp::CMainApp(void)
	: m_pKeyMgr(Engine::Get_KeyMgr())
{

}

CMainApp::~CMainApp(void)
{

}

HRESULT CMainApp::Ready_MainApp(void)
{
	ShowCursor(false);

	FAILED_CHECK_RETURN(SetUp_DefaultSetting(&m_pGrpahicDev), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Resource(m_pGrpahicDev, RESOURCE_END), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Component_Prototype(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_OpeningScene(m_pGrpahicDev, &m_pManagementInstance), E_FAIL);

	// 임시코드
	//_matrix		matView, matProj;

	//D3DXMatrixLookAtLH(&matView,
	//	&_vec3(0.f, 0.f, -5.f),
	//	&_vec3(0.f, 0.f, 0.f),
	//	&_vec3(0.f, 1.f, 0.f));
	//m_pGrpahicDev->SetTransform(D3DTS_VIEW, &matView);

	//D3DXMatrixPerspectiveFovLH(&matProj,
	//	D3DXToRadian(60.f),
	//	_float(WINCX) / WINCY,
	//	0.1f,
	//	1000.f);

	//m_pGrpahicDev->SetTransform(D3DTS_PROJECTION, &matProj);
	return S_OK;
}

_int CMainApp::Update_MainApp(_float fTimeDelta)
{
	if (nullptr == m_pManagementInstance)
		return -1;

	m_fTime += fTimeDelta;

	m_pKeyMgr->CheckKeyInput();
	Engine::Set_InputDev();
	_int iResult = m_pManagementInstance->Update_Scene(fTimeDelta);

	return iResult;
}

void CMainApp::Render_MainApp(void)
{
	++m_dwRenderCnt;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS : %d", m_dwRenderCnt);
		m_fTime = 0.f;
		m_dwRenderCnt = 0;
	}

//	m_pGrpahicDev->SetRenderState(D3DRS_LIGHTING, false);

	if (nullptr == m_pManagementInstance)
		return;

	m_pDeviceInstance->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	Engine::Render_Font(L"Font_Default", m_szFPS, &_vec2(400.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	//
	if (nullptr != m_pRenderer)
		m_pRenderer->Render_GameObject();

	m_pManagementInstance->Render_Scene();

	m_pDeviceInstance->Render_End();

	Engine::Clear_Renderer();
}

HRESULT CMainApp::SetUp_DefaultSetting(LPDIRECT3DDEVICE9 * ppGraphicDev)
{
	FAILED_CHECK_RETURN(Engine::Ready_GraphicDev(g_hWnd,
		Engine::CGraphicDev::MODE_WIN,
		WINCX,
		WINCY,
		&m_pDeviceInstance), E_FAIL);

	m_pDeviceInstance->AddRef();

	*ppGraphicDev = m_pDeviceInstance->GetDevice();
	(*ppGraphicDev)->AddRef();

	// 폰트
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGrpahicDev,
		L"Font_Default",
		L"바탕",
		15,
		20,
		FW_HEAVY), E_FAIL);

	// DInput
	FAILED_CHECK_RETURN(Engine::Ready_InputDev(g_hInst, g_hWnd), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Ready_Resource(LPDIRECT3DDEVICE9 pGraphicDev, RESOURCEID eID)
{
	Engine::Reserve_ContainerSize(eID);

	FAILED_CHECK_RETURN(Engine::Ready_Buffers(m_pDeviceInstance->GetDevice(),
		RESOURCE_STATIC,
		L"Buffer_RcCol",
		Engine::BUFFER_RCCOL),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffers(m_pDeviceInstance->GetDevice(),
		RESOURCE_STATIC,
		L"Buffer_TriCol",
		Engine::BUFFER_TRICOL),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffers(m_pDeviceInstance->GetDevice(),
		RESOURCE_STATIC,
		L"Buffer_RcTex",
		Engine::BUFFER_RCTEX),
		E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Ready_OpeningScene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement ** ppManagementInstance)
{
	Engine::CScene* pScene = nullptr;

	FAILED_CHECK_RETURN(Engine::Create_Management(pGraphicDev, ppManagementInstance), E_FAIL);
	(*ppManagementInstance)->AddRef();

	pScene = CLogo::Create(pGraphicDev);
	NULL_CHECK_RETURN(pScene, E_FAIL);

	FAILED_CHECK_RETURN(Engine::SetUp_CurrentScene(pScene), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Ready_Component_Prototype()
{
	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Renderer", m_pRenderer = Engine::CRenderer::Create(m_pGrpahicDev)), E_FAIL);

//	m_pRenderer->AddRef();
	return NOERROR;
}

CMainApp* CMainApp::Create(void)
{
	CMainApp*		pInstance = new CMainApp;

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("CMainApp Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free(void)
{
	CSoundMgr::GetInstance()->DestroyInstance();
	Engine::Clear_Renderer();
	Engine::Safe_Release(m_pRenderer);
	Engine::Safe_Release(m_pDeviceInstance);
	Engine::Safe_Release(m_pGrpahicDev);
	Engine::Safe_Release(m_pManagementInstance);

	Engine::Release_Utility();
	Engine::Release_Resources();
	Engine::Release_System();

}