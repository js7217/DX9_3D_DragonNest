#include "CellTriCol.h"

USING(Engine)

Engine::CCellTriCol::CCellTriCol(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVIBuffer(pGraphicDev)
{

}

Engine::CCellTriCol::CCellTriCol(const CCellTriCol& rhs)
	:CVIBuffer(rhs)
{

}

Engine::CCellTriCol::~CCellTriCol(void)
{

}

HRESULT Engine::CCellTriCol::Ready_Buffer(const _vec3& vPosA,
										  const _vec3& vPosB,
										  const _vec3& vPosC,
										  const D3DXCOLOR& dwColor)
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

	pVertex[0].vPos = vPosA;
	pVertex[0].dwColor = dwColor;

	pVertex[1].vPos = vPosB;
	pVertex[1].dwColor = dwColor;

	pVertex[2].vPos = vPosC;
	pVertex[2].dwColor = dwColor;

	m_pVB->Unlock();

	INDEX32*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	m_pIB->Unlock();

	return S_OK;
}

void CCellTriCol::Change_Color(const D3DXCOLOR & dwColor)
{
	VTXCOL*			pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].dwColor = dwColor;
	pVertex[1].dwColor = dwColor;
	pVertex[2].dwColor = dwColor;

	m_pVB->Unlock();
}

void CCellTriCol::Set_Pos(const _vec3 & vPosA, const _vec3 & vPosB, const _vec3 & vPosC)
{
	VTXCOL*			pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = vPosA;
	pVertex[1].vPos = vPosB;
	pVertex[2].vPos = vPosC;

	m_pVB->Unlock();
}

Engine::CCellTriCol* Engine::CCellTriCol::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
												const _vec3& vPosA, 
												const _vec3& vPosB, 
												const _vec3& vPosC, 
												const D3DXCOLOR& dwColor)
{
	CCellTriCol* pInstance = new CCellTriCol(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(vPosA, vPosB, vPosC, dwColor)))
	{
		ERR_BOX("CRect Color Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CResources * CCellTriCol::Clone(void)
{
	return new CCellTriCol(*this);
}

void Engine::CCellTriCol::Free(void)
{
	CVIBuffer::Free();
}

