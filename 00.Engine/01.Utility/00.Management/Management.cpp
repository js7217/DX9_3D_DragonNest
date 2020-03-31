#include "Management.h"
#include "ProtoMgr.h"

USING(Engine)

IMPLEMENT_SINGLETON(CManagement)

Engine::CManagement::CManagement(void)
	:m_pCurrentScene(nullptr)
{

}

Engine::CManagement::~CManagement(void)
{
	Free();
}

HRESULT CManagement::Ready_ShaderFile(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShader* pShader = nullptr;
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Sample.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(CProtoMgr::GetInstance()->Ready_Prototype(L"Shader_Sample", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Terrain.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(CProtoMgr::GetInstance()->Ready_Prototype(L"Shader_Terrain", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Mesh.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(CProtoMgr::GetInstance()->Ready_Prototype(L"Shader_Mesh", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_MeshEffect.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(CProtoMgr::GetInstance()->Ready_Prototype(L"Shader_MeshEffect", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_UI.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(CProtoMgr::GetInstance()->Ready_Prototype(L"Shader_UI", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_SkyBox.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(CProtoMgr::GetInstance()->Ready_Prototype(L"Shader_SkyBox", pShader), E_FAIL);

	return S_OK;
}

HRESULT Engine::CManagement::SetUp_CurrentScene(CScene* pCurScene)
{
	Safe_Release(m_pCurrentScene);

	m_pCurrentScene = pCurScene;

	return S_OK;
}

_int Engine::CManagement::Update_Scene(const _float& fTimeDelta)
{
	if (nullptr == m_pCurrentScene)
		return -1;

	_int iResult = m_pCurrentScene->Update_Scene(fTimeDelta);

	return iResult;
}

void Engine::CManagement::Render_Scene()
{
	if (nullptr == m_pCurrentScene)
		return;

	m_pCurrentScene->Render_Scene();
}

CComponent * CManagement::Get_Component(const _tchar * pLayertag, const _tchar * pObjtag, const _tchar * ComponentKey, COMPONENTID eID)
{
	if (nullptr == m_pCurrentScene)
		return nullptr;

	return m_pCurrentScene->Get_Component(pLayertag, pObjtag, ComponentKey, eID);
}

CLayer * CManagement::Get_Layer(const _tchar * pLayertag)
{
	if (nullptr == m_pCurrentScene)
		return nullptr;

	return m_pCurrentScene->Get_Layer(pLayertag);
}

HRESULT CManagement::Delete_GameObject(const _tchar * pLayertag, const _tchar * pObjtag)
{
	if (nullptr == m_pCurrentScene)
		return E_FAIL;

	return m_pCurrentScene->Delete_GameObject(pLayertag, pObjtag);
}

HRESULT CManagement::Erase_GameObject(const _tchar * pLayertag, const _tchar * pObjtag, CGameObject * pGameObject)
{
	if (nullptr == m_pCurrentScene)
		return E_FAIL;

	return m_pCurrentScene->Erase_GameObject(pLayertag, pObjtag, pGameObject);
}

void Engine::CManagement::Free(void)
{
	Safe_Release(m_pCurrentScene);
}
