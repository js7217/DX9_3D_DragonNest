#include "TriCol.h"

USING(Engine)

Engine::CTriCol::CTriCol(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVIBuffer(pGraphicDev)
{

}

Engine::CTriCol::CTriCol(const CTriCol& rhs)
	:CVIBuffer(rhs)
{

}

Engine::CTriCol::~CTriCol(void)
{

}

HRESULT Engine::CTriCol::Ready_Buffer(void)
{
	m_dwVtxCnt = 3;
	m_dwVtxFVF = FVF_COL;
	m_dwTriCnt = 1;
	m_dwVtxSize = sizeof(VTXCOL);

	m_IdxFmt = D3DFMT_INDEX32;
	m_dwIdxSize = sizeof(INDEX32);

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXCOL*			pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = _vec3(0.0f, 0.5f, 0.f);
	pVertex[0].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[1].vPos = _vec3(0.5f, -0.5f, 0.f);
	pVertex[1].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[2].vPos = _vec3(-0.5f, -0.5f, 0.f);
	pVertex[2].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	m_pVB->Unlock();

	INDEX32*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	m_pIB->Unlock();

	return S_OK;
}

Engine::CTriCol* Engine::CTriCol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTriCol* pInstance = new CTriCol(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		ERR_BOX("CRect Color Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CResources * CTriCol::Clone(void)
{
	return new CTriCol(*this);
}

void Engine::CTriCol::Free(void)
{
	CVIBuffer::Free();
}

