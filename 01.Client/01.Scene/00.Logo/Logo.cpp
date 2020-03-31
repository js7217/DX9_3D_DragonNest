#include "stdafx.h"
#include "Logo.h"

#include "Export_Function.h"
#include "Stage.h"
#include "BossStage.h"
CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{

}

CLogo::~CLogo(void)
{

}

HRESULT CLogo::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Engine::CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Resources(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);

	m_pLoading = CLoading::Create(m_pGraphicDev, LOADING_STAGE);
	NULL_CHECK_RETURN(m_pLoading, E_FAIL);

	CSoundMgr::GetInstance()->Initialize();
	
	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_uint iExit = CScene::Update_Scene(fTimeDelta);

	//if (true == m_pLoading->Get_Finish())
	//{
	//	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	//	{
	//		Engine::CScene* pScene = NULL;

	//		pScene = CStage::Create(m_pGraphicDev);
	//		NULL_CHECK_RETURN(pScene, E_FAIL);

	//		if (FAILED(Engine::SetUp_CurrentScene(pScene)))
	//			return E_FAIL;

	//		return iExit;
	//	}
	//}

	return iExit;
}

void CLogo::Render_Scene(void)
{
//	CScene::Render_Scene();// -> Render_Object()
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);

	Engine::Render_Font(L"Font_Default", m_pLoading->Get_String(),
		&_vec2(10.f, 10.f), D3DXCOLOR(0.f, 1.f, 0.f, 1.f));
	if (true == m_pLoading->Get_Finish())
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			Engine::CScene* pScene = NULL;

			pScene = CStage::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pScene, );

			if (FAILED(Engine::SetUp_CurrentScene(pScene)))
				return;

			return;
		}
	}
}

HRESULT CLogo::Ready_Layer_GameLogic(const _tchar* pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;
	// BackGround
	pGameObject = CBackGround::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", pGameObject), E_FAIL);
	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CLogo::Ready_Resources(void)
{
	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_LOGO,
		L"Texture_Back", Engine::TEX_NORMAL, L"../Bin/Resources/Texture/Logo/Logo.jpg"), E_FAIL);

	return S_OK;
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo* pInstance = new CLogo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		ERR_BOX("Logo Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogo::Free(void)
{
	_uint i = Engine::Safe_Release(m_pLoading);
//	Engine::Clear_Renderer();
	CScene::Free();
}
