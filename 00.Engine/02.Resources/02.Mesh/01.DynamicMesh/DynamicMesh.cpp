#include "DynamicMesh.h"

USING(Engine)

CDynamicMesh::CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
	, m_pLoader(nullptr)
	, m_pAniCtrl(nullptr)
{
}

CDynamicMesh::CDynamicMesh(const CDynamicMesh & rhs)
	: CMesh(rhs)
	, m_pLoader(rhs.m_pLoader)
	, m_pRootFrame(rhs.m_pRootFrame)
	, m_MeshContainerList(rhs.m_MeshContainerList)
{
	m_pAniCtrl = CAniCtrl::Create(*rhs.m_pAniCtrl);
}

CDynamicMesh::~CDynamicMesh(void)
{
}

const D3DXFRAME_DERIVED * CDynamicMesh::Get_FrameByName(const char * pFrameName)
{
	return (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pFrameName);
	// 해당 뼈의 이름을 통해 뼈의 정보 구조체를 리턴한다.
	// (즉 해당 뼈 정보를 가져올 수 있고 그 뼈 정보를 이용할것)
}

_bool CDynamicMesh::Is_AnimationSetEnd(_double EndTime)
{
	return m_pAniCtrl->Is_AnimationSetEnd(EndTime);
}

_bool CDynamicMesh::Is_AnimationAfterTrack(_double AfterTime)
{
	return m_pAniCtrl->Is_AnimationAfterTrack(AfterTime);
}

const _uint & CDynamicMesh::Get_AnimationNum()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_pAniCtrl->Get_AnimationNum();
}

_double CDynamicMesh::Get_CurAnimationPosition()
{
	return m_pAniCtrl->Get_CurAnimationPosition();
}

const list<D3DXMESHCONTAINER_DERIVED*>& CDynamicMesh::Get_MeshContainerList()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_MeshContainerList;
}

HRESULT CDynamicMesh::Ready_Meshes(const _tchar * pFilePath, const _tchar * pFileName)
{
	_tchar szFullPath[MAX_PATH] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	m_pLoader = CHierarchy::Create(m_pGraphicDev, pFilePath);
	NULL_CHECK_RETURN(m_pLoader, E_FAIL);

	LPD3DXANIMATIONCONTROLLER pAniCtrl = nullptr;

	FAILED_CHECK_RETURN(D3DXLoadMeshHierarchyFromX(szFullPath,
		D3DXMESH_MANAGED,
		m_pGraphicDev,
		m_pLoader,
		NULL,
		&m_pRootFrame,
		&pAniCtrl), E_FAIL);

	m_pAniCtrl = CAniCtrl::Create(pAniCtrl);
	NULL_CHECK_RETURN(m_pAniCtrl, E_FAIL);

	Safe_Release(pAniCtrl);

	_matrix matTemp;
	Update_FrameMatrix((D3DXFRAME_DERIVED*)m_pRootFrame, D3DXMatrixIdentity(&matTemp));

	SetUp_FrameMatrixPointer((D3DXFRAME_DERIVED*)m_pRootFrame);
	return S_OK;
}

void CDynamicMesh::Render_Meshes(void)
{
	auto iter = m_MeshContainerList.begin();
	auto iter_end = m_MeshContainerList.end();

	for (; iter != iter_end; ++iter)
	{
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = (*iter);

		for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
			pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffeSetMatrix[i] * (*pMeshContainer->ppFrameCombinedMatrix[i]);

		void* pSrcVtx = nullptr;
		void* pDestVtx = nullptr;

		pMeshContainer->pOriMesh->LockVertexBuffer(0, &pSrcVtx);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVtx);

		// 소프트웨어 스키닝을 수행하는 함수(스키닝 뿐 아니라 애니메이션 변경 시, 뼈대들과 정점 정보들의 변경을 동시에 수행해주기도 한다)
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrix,	// 뼈의 최종 변환 상태
			NULL,			// 원상태로 돌려놓기 위한 상태 행렬의 주소값(본래는 뼈대마다 싹 다 역행렬을 구해줘서 넣어줘야하지만 안넣어줘도 전혀 상관 없음)
			pSrcVtx,  // 변하지 않는 원본 메쉬의 정점 정보
			pDestVtx);	// 변환된 정보를 담기 위한 정정 정보

						// 실제 출력 파트

		for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
		{
			m_pGraphicDev->SetTexture(0, pMeshContainer->ppTexture[i]);
			pMeshContainer->MeshData.pMesh->DrawSubset(i);
		}

		pMeshContainer->pOriMesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}
}

void CDynamicMesh::Render_Meshes(LPD3DXEFFECT pEffect)
{
	auto		iter = m_MeshContainerList.begin();
	auto		iter_end = m_MeshContainerList.end();

	for (; iter != iter_end; ++iter)
	{
		D3DXMESHCONTAINER_DERIVED*		pMeshContainer = (*iter);

		for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
			pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffeSetMatrix[i] * (*pMeshContainer->ppFrameCombinedMatrix[i]);

		void*		pSrcVtx = nullptr;
		void*		pDestVtx = nullptr;

		pMeshContainer->pOriMesh->LockVertexBuffer(0, &pSrcVtx);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVtx);
		
		// 소프트웨어 스키닝을 수행하는 함수(스키닝 뿐 아니라 애니메이션 변경 시, 뼈대들과 정점 정보들의 변경을 동시에 수행해주기도 한다)
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrix,	// 뼈의 최종 변환 상태
			NULL,			// 원상태로 돌려놓기 위한 상태 행렬의 주소값(본래는 뼈대마다 싹 다 역행렬을 구해줘서 넣어줘야하지만 안넣어줘도 전혀 상관 없음)
			pSrcVtx,  // 변하지 않는 원본 메쉬의 정점 정보
			pDestVtx);	// 변환된 정보를 담기 위한 정정 정보

						// 실제적인 출력 파트

		for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
		{
			pEffect->SetTexture("g_BaseTexture", pMeshContainer->ppTexture[i]);

			pEffect->CommitChanges();		// 변경된 텍스쳐의 내용으로 세팅될 수 있도록 내부 메모리를 정리해주는 함수

			pMeshContainer->MeshData.pMesh->DrawSubset(i);
		}

		pMeshContainer->pOriMesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}
}

void CDynamicMesh::Set_AnimationSet(const _uint & iIndex)
{
	m_pAniCtrl->Set_AnimationSet(iIndex);
}

void CDynamicMesh::Play_AnimationSet(const _float & fTimeDelta)
{
	m_pAniCtrl->Play_AnimationSet(fTimeDelta);

	_matrix matTemp;

	Update_FrameMatrix((D3DXFRAME_DERIVED*)m_pRootFrame, D3DXMatrixIdentity(&matTemp));
}

void CDynamicMesh::Update_FrameMatrix(D3DXFRAME_DERIVED * pFrame, const _matrix * pParentMatrix)
{
	if (nullptr == pFrame)
		return; //부모가 없는 경우 종료한다.

	pFrame->CombinedTransformMatrix = pFrame->TransformationMatrix * (*pParentMatrix);

	if (nullptr != pFrame->pFrameSibling)
		Update_FrameMatrix((D3DXFRAME_DERIVED*)pFrame->pFrameSibling, pParentMatrix);

	if (nullptr != pFrame->pFrameFirstChild)
		Update_FrameMatrix((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild, &pFrame->CombinedTransformMatrix);
}

void CDynamicMesh::SetUp_FrameMatrixPointer(D3DXFRAME_DERIVED * pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
		{
			const char* pBonName = pMeshContainer->pSkinInfo->GetBoneName(i);
			// 뼈의 이름을 가져온다.
			D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED *)D3DXFrameFind(m_pRootFrame, pBonName);
			//  D3DXFrameFind를 통해 뼈대 이름으로 뼈대를 가져올 수있다.

			pMeshContainer->ppFrameCombinedMatrix[i] = &pFrame->CombinedTransformMatrix;
		}

		m_MeshContainerList.emplace_back(pMeshContainer);
	}

	if (nullptr != pFrame->pFrameSibling)
		SetUp_FrameMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameSibling);

	if (nullptr != pFrame->pFrameFirstChild)
		SetUp_FrameMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild);
}

CDynamicMesh * CDynamicMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, const _tchar * pFileName)
{
	CDynamicMesh* pInstance = new CDynamicMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Meshes(pFilePath, pFileName)))
		Safe_Release(pInstance);
	return pInstance;
}

CResources * CDynamicMesh::Clone(void)
{
	return new CDynamicMesh(*this);
}

void CDynamicMesh::Free(void)
{
	CMesh::Free();
	Safe_Release(m_pAniCtrl);

	if (false == m_bClone)
	{
		m_pLoader->DestroyFrame(m_pRootFrame);
		Safe_Release(m_pLoader);
	}

	m_MeshContainerList.clear();
}
