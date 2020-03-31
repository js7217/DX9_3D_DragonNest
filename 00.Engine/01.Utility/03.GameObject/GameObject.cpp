#include "GameObject.h"

USING(Engine)


Engine::CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_eCurState(STAND), m_ePreState(STAND)
	, m_iHp(10)
{
	m_pGraphicDev->AddRef();
	D3DXMatrixIdentity(&m_matBone);
}

Engine::CGameObject::~CGameObject(void)
{

}

void CGameObject::Set_Hit()
{
	m_iHp--;
	m_bIsHit = true;

	this->Set_HitEffect();
}

HRESULT Engine::CGameObject::Ready_Object(void)
{
	return S_OK;
}

_int Engine::CGameObject::Update_Object(const _float& fTimeDelta)
{
	for (auto& iter : m_mapComponent[ID_DYNAMIC])
		iter.second->Update_Component(fTimeDelta);

	return 0;
}

void Engine::CGameObject::Render_Object(void)
{

}

void Engine::CGameObject::Render_Shadow(void)
{
}

void CGameObject::Compute_ViewZ(const _vec3 * pPos)
{
	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixInverse(&matView, 0, &matView);

	_vec3 vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(_vec3));

	m_fViewZ = D3DXVec3Length(&(vCamPos - *pPos));
}

CComponent * CGameObject::Get_Component(const _tchar * ComponentKey, COMPONENTID eID)
{
	CComponent* pComponent = Find_Component(ComponentKey, eID);

	return pComponent;
}

HRESULT CGameObject::Delete_Component(const _tchar * ComponentKey, COMPONENTID eID)
{
	auto iter = m_mapComponent[eID].find(ComponentKey);
	//	auto iter = find_if(m_mapComponent[eID].begin(), m_mapComponent[eID].end(), CTag_Finder(ComponentKey));

	if (iter == m_mapComponent[eID].end())
		return E_FAIL;

	Safe_Release(iter->second);
	m_mapComponent[eID].erase(iter);

	return S_OK;
}

_bool CGameObject::CollisionSphere(const _matrix & matSphere, const _float& fTargetRadius)
{
	_vec3 vPos, vTargetPos = {};

	memcpy(&vPos, m_matBone.m[3], sizeof(_vec3)); // vPos 값 복사.
	memcpy(&vTargetPos, matSphere.m[3], sizeof(_vec3)); // vTargetPos 값 복사.

	_float fRadiusDist = fTargetRadius + m_fRadius; // 반지름의 합
	_float fLength = D3DXVec3Length(&(vTargetPos - vPos)); // 현재 거리.
	
	return fRadiusDist >= fLength;
}

CComponent * CGameObject::Find_Component(const _tchar * ComponentKey, COMPONENTID eID)
{
	auto iter = m_mapComponent[eID].find(ComponentKey);
//	auto iter = find_if(m_mapComponent[eID].begin(), m_mapComponent[eID].end(), CTag_Finder(ComponentKey));

	if (iter == m_mapComponent[eID].end())
		return nullptr;

	return iter->second;
}

void Engine::CGameObject::Free(void)
{
	Safe_Release(m_pGraphicDev);

	for (_ulong i = 0; i < ID_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
		m_mapComponent[i].clear();
	}
}
