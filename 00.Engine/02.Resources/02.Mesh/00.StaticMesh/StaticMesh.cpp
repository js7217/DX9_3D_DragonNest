#include "StaticMesh.h"

USING(Engine)

Engine::CStaticMesh::CStaticMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
{

}

Engine::CStaticMesh::CStaticMesh(const CStaticMesh& rhs)
	: CMesh(rhs)
	, m_pAdjacency(rhs.m_pAdjacency)
	, m_pSubsetBuffer(rhs.m_pSubsetBuffer)
	, m_pSubsets(rhs.m_pSubsets)
	, m_dwSubsetCnt(rhs.m_dwSubsetCnt)
	, m_pMesh(rhs.m_pMesh)
	, m_pOriMesh(rhs.m_pOriMesh)
	, m_dwNumVtx(rhs.m_dwNumVtx)
	, m_dwStride(rhs.m_dwStride)
	, m_pVtxPos(rhs.m_pVtxPos)
{
	m_ppTextures = new LPDIRECT3DTEXTURE9[rhs.m_dwSubsetCnt];

	for (_ulong i = 0; i < rhs.m_dwSubsetCnt; ++i)
	{
		m_ppTextures[i] = rhs.m_ppTextures[i];
		m_ppTextures[i]->AddRef();
	}
	m_pAdjacency->AddRef();
	m_pSubsetBuffer->AddRef();
	m_pMesh->AddRef();
	m_pOriMesh->AddRef();
}

Engine::CStaticMesh::~CStaticMesh(void)
{

}

HRESULT Engine::CStaticMesh::Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName)
{
	_tchar szFullPath[128] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	if (FAILED(D3DXLoadMeshFromX(szFullPath,
		D3DXMESH_MANAGED,
		m_pGraphicDev,
		&m_pAdjacency,
		&m_pSubsetBuffer,
		NULL,
		&m_dwSubsetCnt,
		&m_pOriMesh)))
		return E_FAIL;

	_ulong dwFVF = m_pOriMesh->GetFVF();

	if (!(dwFVF & D3DFVF_NORMAL))
	{
		m_pOriMesh->CloneMeshFVF(m_pOriMesh->GetOptions(), dwFVF || D3DFVF_NORMAL, m_pGraphicDev, &m_pMesh);

		D3DXComputeNormals(m_pMesh, (_ulong*)m_pAdjacency->GetBufferPointer());
	}
	else
	{
		m_pOriMesh->CloneMeshFVF(m_pOriMesh->GetOptions(), dwFVF, m_pGraphicDev, &m_pMesh);
	}

	void*	pVertex = NULL;

	m_dwNumVtx = m_pMesh->GetNumVertices();

	m_pVtxPos = new _vec3[m_dwNumVtx];

	m_pMesh->LockVertexBuffer(0, &pVertex);

	D3DVERTEXELEMENT9	Decl[MAX_FVF_DECL_SIZE];
	ZeroMemory(Decl, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);

	m_pMesh->GetDeclaration(Decl);

	_ubyte byOffset = 0;

	for (_ulong i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (Decl[i].Usage == D3DDECLUSAGE_POSITION)
		{
			byOffset = (_ubyte)Decl[i].Offset;
			break;
		}
	}

	m_dwStride = D3DXGetFVFVertexSize(dwFVF);

	for (_ulong i = 0; i < m_dwNumVtx; ++i)
	{
		m_pVtxPos[i] = *((_vec3*)(((_ubyte*)pVertex) + (m_dwStride * i + byOffset)));

	}

	m_pMesh->UnlockVertexBuffer();

	m_pSubsets = (D3DXMATERIAL*)m_pSubsetBuffer->GetBufferPointer();

	m_ppTextures = new LPDIRECT3DTEXTURE9[m_dwSubsetCnt];

	for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
	{
		_tchar szFileName[128] = L"";

		lstrcpy(szFullPath, pFilePath);
		MultiByteToWideChar(CP_ACP, 0, m_pSubsets[i].pTextureFilename, strlen(m_pSubsets[i].pTextureFilename), szFileName, 128);

		lstrcat(szFullPath, szFileName);

		if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppTextures[i])))
			return E_FAIL;
	}
	return S_OK;
}

void Engine::CStaticMesh::Render_Meshes(void)
{
	for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
	{
		m_pGraphicDev->SetTexture(0, m_ppTextures[i]);
		m_pMesh->DrawSubset(i);
	}
}

void CStaticMesh::Render_Meshes(LPD3DXEFFECT pEffect)
{
	for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
	{
		_bool bAlpha = false;
		_uint iPass = 0;

		if (bAlpha = Find_Alpha(m_pSubsets[i].pTextureFilename))
			iPass = 1;

		pEffect->SetTexture("g_BaseTexture", m_ppTextures[i]);
		pEffect->CommitChanges();

		pEffect->BeginPass(iPass);

		m_pMesh->DrawSubset(i);

		pEffect->EndPass();
	}
}

_bool CStaticMesh::Find_Alpha(const char * pFileName)
{
	_uint iLength = strlen(pFileName);

	for (_uint i = 0; i < iLength; ++i)
	{
		if (pFileName[i] == '.')
		{
			if (pFileName[i - 1] == 'A')
				return true;
		}
	}

	return false;
}

CStaticMesh* Engine::CStaticMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName)
{
	CStaticMesh*	pInstance = new CStaticMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Meshes(pFilePath, pFileName)))
	{
		ERR_BOX("StaticMesh Created Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

CResources* Engine::CStaticMesh::Clone(void)
{
	return new CStaticMesh(*this);
}

void Engine::CStaticMesh::Free(void)
{
	Engine::CMesh::Free();

	for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
	{
		Engine::Safe_Release(m_ppTextures[i]);
	}

	Engine::Safe_Release(m_pAdjacency);
	Engine::Safe_Release(m_pSubsetBuffer);
	Engine::Safe_Release(m_pMesh);
	Engine::Safe_Release(m_pOriMesh);

	if (false == m_bClone)
		Safe_Delete_Array(m_pVtxPos);

	Safe_Delete_Array(m_ppTextures);
}

