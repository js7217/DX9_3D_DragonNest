#include "TerrainTex.h"

USING(Engine)

Engine::CTerrainTex::CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
	, m_pPos(nullptr)
	, m_bClone(false)
{

}

Engine::CTerrainTex::CTerrainTex(const CTerrainTex& rhs)
	:CVIBuffer(rhs)
	, m_pPos(rhs.m_pPos)
	, m_bClone(true)
	, m_fH(rhs.m_fH)
	, m_iH(rhs.m_iH)
	, m_dwCntX(rhs.m_dwCntX)
	, m_dwCntZ(rhs.m_dwCntZ)
{

}

Engine::CTerrainTex::~CTerrainTex(void)
{

}

void CTerrainTex::Copy_Index(Engine::INDEX32 * pIndex, const _ulong & dwTriCnt)
{
	Engine::INDEX32* pOriginIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pOriginIndex, 0);

	memcpy(pOriginIndex, pIndex, sizeof(Engine::INDEX32) * dwTriCnt);

	m_pIB->Unlock();

	m_dwTriCnt = dwTriCnt;
}

HRESULT Engine::CTerrainTex::Ready_Buffer(const _ulong& dwCntX,
	const _ulong& dwCntZ,
	const _ulong& dwVtxItv,
	const _ulong& dwDetail)
{
	///////////////////////////////////////////////////////////////////////
	/*LPDIRECT3DTEXTURE9			pTexture = nullptr;

	D3DXCreateTexture(m_pGraphicDev, 129, 129, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture);

	D3DLOCKED_RECT LockRect;

	pTexture->LockRect(0, &LockRect, NULL, 0);

	for (_int i = 0; i < 129; ++i)
	{
	for (_int j = 0; j < 129; ++j)
	{
	_int	iIndex = i * 129 + j;

	*(((_ulong*)LockRect.pBits) + iIndex) = D3DCOLOR_ARGB(255, 255, 0, 0);
	}
	}

	pTexture->UnlockRect(0);

	D3DXSaveTextureToFile(L"../Bin/Resources/Texture/Terrain/TestHeight.bmp", D3DXIFF_BMP, pTexture, NULL);*/



	m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;
	m_dwVtxCnt = dwCntX * dwCntZ;

	m_pPos = new _vec3[m_dwVtxCnt];

	m_dwVtxFVF = FVF_TEX;
	m_dwVtxSize = sizeof(VTXTEX);

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK(CVIBuffer::Ready_Buffer());

	_ulong		dwByte = 0;

	m_hFile = CreateFile(L"../Bin/Resources/Texture/Terrain/Height.bmp",
		GENERIC_READ,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	ReadFile(m_hFile, &m_fH, sizeof(BITMAPFILEHEADER), &dwByte, NULL);
	ReadFile(m_hFile, &m_iH, sizeof(BITMAPINFOHEADER), &dwByte, NULL);

	_ulong*		pPixel = new _ulong[m_iH.biWidth * m_iH.biHeight];

	ReadFile(m_hFile, pPixel, sizeof(_ulong) * m_iH.biHeight * m_iH.biWidth, &dwByte, NULL);

	CloseHandle(m_hFile);


	VTXTEX*		pVtxTex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVtxTex, 0);

	_ulong		dwIndex = 0;

	for (_ulong i = 0; i < dwCntZ; ++i)
	{
		for (_ulong j = 0; j < dwCntX; ++j)
		{
			dwIndex = i * dwCntX + j;

			pVtxTex[dwIndex].vPos = _vec3(_float(j) * dwVtxItv,
				/*(pPixel[dwIndex] & 0x000000ff) / 20.f*/0.f,
				_float(i) * dwVtxItv);

			pVtxTex[dwIndex].vNormal = _vec3(0.f, 0.f, 0.f);

			m_pPos[dwIndex] = pVtxTex[dwIndex].vPos;

			pVtxTex[dwIndex].vTexUV.x = (dwDetail - pVtxTex[dwIndex].vPos.x / ((dwCntX - 1) * dwVtxItv)) * dwDetail;
			pVtxTex[dwIndex].vTexUV.y = (dwDetail - pVtxTex[dwIndex].vPos.z / ((dwCntZ - 1) * dwVtxItv)) * dwDetail;
		}
	}

	Safe_Delete_Array(pPixel);

	INDEX32*		pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	_ulong	dwTriIdx = 0;

	for (_ulong i = 0; i < dwCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwCntX - 1; ++j)
		{
			dwIndex = i * dwCntX + j;

			pIndex[dwTriIdx]._0 = dwIndex + dwCntX;
			pIndex[dwTriIdx]._1 = dwIndex + dwCntX + 1;
			pIndex[dwTriIdx]._2 = dwIndex + 1;

			_vec3 vDest, vSour, vNormal;

			vDest = pVtxTex[pIndex[dwTriIdx]._1].vPos - pVtxTex[pIndex[dwTriIdx]._0].vPos;
			vSour = pVtxTex[pIndex[dwTriIdx]._2].vPos - pVtxTex[pIndex[dwTriIdx]._1].vPos;
			D3DXVec3Cross(&vNormal, &vDest, &vSour);

			pVtxTex[pIndex[dwTriIdx]._0].vNormal += vNormal;
			pVtxTex[pIndex[dwTriIdx]._1].vNormal += vNormal;
			pVtxTex[pIndex[dwTriIdx]._2].vNormal += vNormal;

			++dwTriIdx;


			pIndex[dwTriIdx]._0 = dwIndex + dwCntX;
			pIndex[dwTriIdx]._1 = dwIndex + 1;
			pIndex[dwTriIdx]._2 = dwIndex;

			vDest = pVtxTex[pIndex[dwTriIdx]._1].vPos - pVtxTex[pIndex[dwTriIdx]._0].vPos;
			vSour = pVtxTex[pIndex[dwTriIdx]._2].vPos - pVtxTex[pIndex[dwTriIdx]._1].vPos;
			D3DXVec3Cross(&vNormal, &vDest, &vSour);

			pVtxTex[pIndex[dwTriIdx]._0].vNormal += vNormal;
			pVtxTex[pIndex[dwTriIdx]._1].vNormal += vNormal;
			pVtxTex[pIndex[dwTriIdx]._2].vNormal += vNormal;

			++dwTriIdx;
		}
	}

	for (_ulong i = 0; i < m_dwVtxCnt; ++i)
		D3DXVec3Normalize(&pVtxTex[i].vNormal, &pVtxTex[i].vNormal);

	m_pVB->Unlock();
	m_pIB->Unlock();

	return S_OK;
}

CTerrainTex* Engine::CTerrainTex::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	const _ulong& dwCntX,
	const _ulong& dwCntZ,
	const _ulong& dwVtxItv, 
	const _ulong& dwDetail)
{
	CTerrainTex*		pInstance = new CTerrainTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv, dwDetail)))
		Engine::Safe_Release(pInstance);

	pInstance->Set_Cnt(dwCntX, dwCntZ);

	return pInstance;
}

void Engine::CTerrainTex::Free(void)
{
	CVIBuffer::Free();

	if (false == m_bClone)
	{
		Safe_Delete_Array(m_pPos);
	}

}

Engine::CResources* Engine::CTerrainTex::Clone(void)
{
	return new CTerrainTex(*this);
}

