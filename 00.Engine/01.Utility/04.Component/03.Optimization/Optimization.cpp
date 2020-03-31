#include "Optimization.h"

USING(Engine)

COptimization::COptimization(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pFrustum(nullptr)
	, m_bClone(false)
{
	m_pGraphicDev->AddRef();
}

COptimization::COptimization(const COptimization & rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
	, m_pFrustum(rhs.m_pFrustum)
	, m_bClone(true)
{
	m_pGraphicDev->AddRef();
}

COptimization::~COptimization(void)
{
}

HRESULT COptimization::Ready_Optimization()
{
	m_pFrustum = CFrustum::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(m_pFrustum, E_FAIL);

	return S_OK;
}

_bool COptimization::IsInFrustum_ForObject(const _vec3 * pPosWorld, const _float fRadius)
{
	return m_pFrustum->IsInFrustum_ForObject(pPosWorld, fRadius);
}

COptimization * COptimization::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	COptimization* pInstance = new COptimization(pGraphicDev);

	if (FAILED(pInstance->Ready_Optimization()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * COptimization::Clone(void)
{
	return new COptimization(*this);
}

void COptimization::Free(void)
{
	if (false == m_bClone)
	{
		Safe_Release(m_pFrustum);
	}

	Safe_Release(m_pGraphicDev);
}
