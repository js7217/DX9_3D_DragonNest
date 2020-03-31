#include "ProtoMgr.h"

USING(Engine)
#include "Renderer.h"
IMPLEMENT_SINGLETON(CProtoMgr)

CProtoMgr::CProtoMgr(void)
{
}

CProtoMgr::~CProtoMgr(void)
{
	Free();
}

HRESULT CProtoMgr::Ready_Prototype(const _tchar * pProtoTag, CComponent * pInstance)
{
	CComponent* pProtoType = Find_Prototype(pProtoTag);

	if (nullptr != pProtoType)
		return E_FAIL;

	m_mapProto.emplace(pProtoTag, pInstance);

	return S_OK;
}

CComponent * CProtoMgr::Clone(const _tchar * pProtoTag)
{
	CComponent* pProtoType = Find_Prototype(pProtoTag);

	if (nullptr == pProtoType)
		return nullptr;

	return pProtoType->Clone();
}

HRESULT CProtoMgr::Clear_Renderer()
{
	CComponent* pProtoType = Find_Prototype(L"Renderer");

	if (nullptr == pProtoType)
		return E_FAIL;


	return dynamic_cast<Engine::CRenderer*>(pProtoType)->Clear_Renderer();
}

CComponent * CProtoMgr::Find_Prototype(const _tchar * pProtoTag)
{
	auto iter = find_if(m_mapProto.begin(), m_mapProto.end(), CTag_Finder(pProtoTag));

	if (iter == m_mapProto.end())
		return nullptr;

	return iter->second;
}

void CProtoMgr::Free(void)
{
	for_each(m_mapProto.begin(), m_mapProto.end(), CDeleteMap());
	m_mapProto.clear();
}
