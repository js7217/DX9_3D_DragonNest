#include "Scene.h"

USING(Engine)

Engine::CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
	:m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CScene::~CScene(void)
{

}

HRESULT Engine::CScene::Ready_Scene(void)
{
	return S_OK;
}

_int Engine::CScene::Update_Scene(const _float& fTimeDelta)
{
	_int isEnd = 0;

	for (auto& iter : m_mapLayer)
	{
		isEnd = iter.second->Update_Layer(fTimeDelta);

		if (isEnd & 0x80000000)
			return -1;
	}

	return isEnd;
}

void Engine::CScene::Render_Scene(void)
{
	//for (auto& iter : m_mapLayer)
	//	iter.second->Render_Layer();
}

CComponent * CScene::Get_Component(const _tchar * pLayertag, const _tchar * pObjtag, const _tchar * ComponentKey, COMPONENTID eID)
{
	auto iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayertag));

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second->Get_Component(pObjtag, ComponentKey, eID);
}

CLayer * CScene::Get_Layer(const _tchar * pLayertag)
{
	auto iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayertag));

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second;
}

HRESULT CScene::Delete_GameObject(const _tchar * pLayertag, const _tchar * pObjtag)
{
	auto iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayertag));

	if (iter == m_mapLayer.end())
		return E_FAIL;

	return iter->second->Delete_GameObject(pObjtag);
}

HRESULT CScene::Erase_GameObject(const _tchar * pLayertag, const _tchar * pObjtag, CGameObject * pGameObject)
{
	auto iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayertag));

	if (iter == m_mapLayer.end())
		return E_FAIL;

	return iter->second->Erase_GameObject(pObjtag, pGameObject);
}

void Engine::CScene::Free(void)
{
	Safe_Release(m_pGraphicDev);

	for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
	m_mapLayer.clear();
}
