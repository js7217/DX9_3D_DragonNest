#include "ResourcesMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CResourcesMgr)


Engine::CResourcesMgr::CResourcesMgr(void)
	:m_pmapResources(nullptr)
{

}

Engine::CResourcesMgr::~CResourcesMgr(void)
{
	Free();
}

HRESULT Engine::CResourcesMgr::Reserve_ContainerSize(const _ushort& wSize)
{
	//  초기화 단계 즉 얼마만큼의 리소스들을 관리할지 결정.

	if (nullptr != m_pmapResources)
		return E_FAIL;

	m_pmapResources = new MAPRESOURCES[wSize];

	m_iContainerSize = wSize;

	return S_OK;
}

HRESULT Engine::CResourcesMgr::Ready_Buffers(LPDIRECT3DDEVICE9 pGraphicDev, 
											const _ushort& wContainerIdx, 
											const _tchar* pBufferTag,
											BUFFERID eID, 
											const WORD & wCntX, const WORD & wCntY, const WORD & wItv, const WORD & wDetail)
{
	if (nullptr == m_pmapResources)
	{
		ERR_BOX("Resources Container not Reserved");
		return E_FAIL;
	}

	CResources* pResources = Find_Resources(wContainerIdx, pBufferTag);

	if (nullptr != pResources)
		return E_FAIL;

	switch (eID)
	{
	case Engine::BUFFER_RCCOL:
		pResources = CRcCol::Create(pGraphicDev);
		break;
	case Engine::BUFFER_RCTEX:
		pResources = CRcTex::Create(pGraphicDev);
		break;
	case Engine::BUFFER_TRICOL:
		pResources = CTriCol::Create(pGraphicDev);
		break;
	case Engine::BUFFER_TERRAINTEX:
		pResources = CTerrainTex::Create(pGraphicDev, wCntX, wCntY, wItv, wDetail);
		break;
	case Engine::BUFFER_CUBETEX:
		pResources = CCubeTex::Create(pGraphicDev);
		break;
	default:
		break;
	}
	NULL_CHECK_RETURN(pResources, E_FAIL);

	m_pmapResources[wContainerIdx].emplace(pBufferTag, pResources);

	return S_OK;
}

HRESULT CResourcesMgr::Ready_Textures(LPDIRECT3DDEVICE9 pGraphicDev, const _ushort & wContainerIdx, const _tchar * pTextureTag, TEXTURETYPE eType, const _tchar * pPath, const _uint & iCnt)
{
	if (nullptr == m_pmapResources)
	{
		ERR_BOX("Resources Container not Reserved");
		return E_FAIL;
	}

	CResources*		pResources = Find_Resources(wContainerIdx, pTextureTag);

	if (nullptr != pResources)
		return E_FAIL;

	pResources = CTexture::Create(pGraphicDev, pPath, eType, iCnt);
	if (nullptr == pResources)
		return E_FAIL;

	m_pmapResources[wContainerIdx].emplace(pTextureTag, pResources);

	return S_OK;
}

HRESULT CResourcesMgr::Ready_Meshes(LPDIRECT3DDEVICE9 pGraphicDev, const _ushort & wContainerIdx, const _tchar * pMeshTag, MESHTYPE eType, const _tchar * pFilePath, const _tchar * pFileName)
{
	if (nullptr == m_pmapResources)
	{
		ERR_BOX("Resources Container not Reserved");
		return E_FAIL;
	}

	CResources*	pResources = Find_Resources(wContainerIdx, pMeshTag);

	if (nullptr != pResources)
		return E_FAIL;

	switch (eType)
	{
	case TYPE_STATIC:
		pResources = CStaticMesh::Create(pGraphicDev, pFilePath, pFileName);
		break;

	case TYPE_DYNAMIC:
		pResources = CDynamicMesh::Create(pGraphicDev, pFilePath, pFileName);
		break;
	case TYPE_NAVI:
		pResources = CNaviMesh::Create(pGraphicDev);
		break;
	}

	if (nullptr == pResources)
		return E_FAIL;

	m_pmapResources[wContainerIdx].emplace(pMeshTag, pResources);

	return S_OK;
}

void Engine::CResourcesMgr::Render_Buffers(const _ushort& wContainerIdx,
												const _tchar* pBufferTag)
{
	CResources* pResources = Find_Resources(wContainerIdx, pBufferTag);

	if (nullptr == pResources)
		return;

	dynamic_cast<CVIBuffer*>(pResources)->Render_Buffer();
}

Engine::CResources* Engine::CResourcesMgr::Find_Resources(const _ushort& wContainerIdx, const _tchar* pResourcesTag)
{
	auto iter = find_if(m_pmapResources[wContainerIdx].begin(),
						m_pmapResources[wContainerIdx].end(),
						CTag_Finder(pResourcesTag));

	if (m_pmapResources[wContainerIdx].end() == iter)
		return nullptr;

	return iter->second;
}

HRESULT CResourcesMgr::Delete_Resources(const _ushort & wContainerIdx, const _tchar * pResourcesTag)
{
	auto iter = find_if(m_pmapResources[wContainerIdx].begin(),
		m_pmapResources[wContainerIdx].end(),
		CTag_Finder(pResourcesTag));

	if (m_pmapResources[wContainerIdx].end() == iter)
		return E_FAIL;

	Safe_Release(iter->second);
	m_pmapResources[wContainerIdx].erase(iter);

	return S_OK;
}

CResources * CResourcesMgr::Clone_Resources(const _ushort & wContainerIdx, const _tchar * pResourceTag)
{
	if (nullptr == m_pmapResources)
		return nullptr;

	CResources* pInstance = Find_Resources(wContainerIdx, pResourceTag);

	return pInstance->Clone();
}

void Engine::CResourcesMgr::Free(void)
{

	for (size_t i = 0; i < m_iContainerSize; ++i)
	{
		for_each(m_pmapResources[i].begin(), m_pmapResources[i].end(), CDeleteMap());
		m_pmapResources[i].clear();
	}

	Safe_Delete_Array(m_pmapResources);
}