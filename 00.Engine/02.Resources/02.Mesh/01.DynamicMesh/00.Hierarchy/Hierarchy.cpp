#include "Hierarchy.h"

USING(Engine)

CHierarchy::CHierarchy(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pPath)
	: m_pGraphicDev(pGraphicDev)
	, m_pPath(pPath)
{
	m_pGraphicDev->AddRef();
}

CHierarchy::~CHierarchy(void)
{
}

STDMETHODIMP CHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	D3DXFRAME_DERIVED*		pNewFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pNewFrame, sizeof(D3DXFRAME_DERIVED));

	Allocate_Name(&pNewFrame->Name, Name);

	pNewFrame->CombinedTransformMatrix = *D3DXMatrixIdentity(&pNewFrame->TransformationMatrix);

	*ppNewFrame = pNewFrame;

	return S_OK;
}

STDMETHODIMP CHierarchy::CreateMeshContainer(LPCSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	Allocate_Name(&pMeshContainer->Name, Name);

	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	LPD3DXMESH pMesh = pMeshData->pMesh;

	_ulong dwNumFaces = pMesh->GetNumFaces(); // 메쉬가 지닌 폴리곤 개수 반환

	pMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3];
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumFaces * 3);

	_ulong dwFVF = pMesh->GetFVF();

	if (!(dwFVF & D3DFVF_NORMAL))
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(),
			dwFVF | D3DFVF_NORMAL,
			m_pGraphicDev,
			&pMeshContainer->MeshData.pMesh);
		D3DXComputeNormals(pMeshContainer->MeshData.pMesh, pMeshContainer->pAdjacency);
	}
	else
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF, m_pGraphicDev, &pMeshContainer->MeshData.pMesh);

	pMeshContainer->NumMaterials = (NumMaterials == 0 ? 1 : NumMaterials);
	// 재질의 개수를 채워주는데 없는 경우도 있기 때문에 조건문을 달아준다.
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	ZeroMemory(pMeshContainer->pMaterials, sizeof(D3DXMATERIAL) * pMeshContainer->NumMaterials);
	// 재질 동적할당(재질개수만큼)
	pMeshContainer->ppTexture = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	ZeroMemory(pMeshContainer->ppTexture, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);
	// 텍스쳐 동적할당(재질개수만큼)

	if (0 != NumMaterials) //재질이 있는 경우
	{
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL)* pMeshContainer->NumMaterials);

		for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
		{
			_tchar		szFullPath[128] = L"";
			_tchar		szFileName[128] = L"";

			MultiByteToWideChar(CP_ACP, 0,
				pMeshContainer->pMaterials[i].pTextureFilename,
				strlen(pMeshContainer->pMaterials[i].pTextureFilename),
				szFileName,
				128);

			lstrcpy(szFullPath, m_pPath);
			lstrcat(szFullPath, szFileName);
			// 해당 재질의 이름은 X파일에 있는데
			// 이는 아스키코드로 작성되어 있기때문에 유니코드로 변환 하고
			// 텍스쳐를 넣어주는 작업을 한다.
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pMeshContainer->ppTexture[i])))
			{
				ERR_BOX("DynamicMesh`s Texture Create Failed");
				return E_FAIL;
			}

		}
	}
	else // 재질이 없는 경우
	{
		pMeshContainer->pMaterials[0].MatD3D.Diffuse = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pMeshContainer->pMaterials[0].MatD3D.Specular = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pMeshContainer->pMaterials[0].MatD3D.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pMeshContainer->pMaterials[0].MatD3D.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pMeshContainer->pMaterials[0].MatD3D.Power = 0.f;
		// 재질이 없을 땐 직접 채워넣어준다(0으로 통일 -> 검은색 재질)
		// 따지고 보면 확인용이기 때문에 확인 뒤 메쉬를 바꾸는 것이 현명..
		// 텍스쳐는 없는 것으로 함.
		pMeshContainer->ppTexture[0] = NULL;
	}

	if (nullptr == pSkinInfo) //  Skinned Mesh가 아닌경우. 그냥 리턴하게 한다.
		return S_OK;

	pMeshContainer->pSkinInfo = pSkinInfo; // 들어온 pSkinInfo 를 내가 제작할 곳에 넣는다.
	pMeshContainer->pSkinInfo->AddRef();

	pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(),
		pMeshContainer->MeshData.pMesh->GetFVF(),
		m_pGraphicDev,
		&pMeshContainer->pOriMesh);
	// Render시 사용하는 메쉬를 만들기 위해.

	// 이 메시 컨테이너에 영향을 미치는 뼈의 개수를 얻어온다.
	pMeshContainer->dwNumBones = pMeshContainer->pSkinInfo->GetNumBones();

	// 최초의 뼈 행렬 상태 값을 저장하기 위한 동적 배열
	pMeshContainer->pFrameOffeSetMatrix = new _matrix[pMeshContainer->dwNumBones];
	ZeroMemory(pMeshContainer->pFrameOffeSetMatrix, sizeof(_matrix) * pMeshContainer->dwNumBones);

	// 메쉬를 그리기 위한 뼈대들에는 인덱스 값이 매겨져 있다. 이런 뼈대들을 순회하면서 행렬 정보를 차례대로 대입해주는 코드이다.
	for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
		pMeshContainer->pFrameOffeSetMatrix[i] = *pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);

	// 뼈대들이 갖고 있는 CombinedMatrix를 매번 가져와서 곱해주는 연산 작업을 원할히 수행하기 위해 저장해두는 동적 배열
	pMeshContainer->ppFrameCombinedMatrix = new _matrix*[pMeshContainer->dwNumBones];
	ZeroMemory(pMeshContainer->ppFrameCombinedMatrix, sizeof(_matrix*) * pMeshContainer->dwNumBones);

	// 결론적으로 위에서 할당해 놓은 pMeshContainer->pFrameOffeSetMatrix 와 pMeshContainer->ppFrameCombinedMatrix의 곱셉의 결과 행렬
	pMeshContainer->pRenderingMatrix = new _matrix[pMeshContainer->dwNumBones];
	ZeroMemory(pMeshContainer->pRenderingMatrix, sizeof(_matrix) * pMeshContainer->dwNumBones);


	*ppNewMeshContainer = pMeshContainer;

	return S_OK;
}

STDMETHODIMP CHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	Safe_Delete_Array(pFrameToFree->Name);

	if (nullptr != pFrameToFree->pMeshContainer)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	if (nullptr != pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	if (nullptr != pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	Safe_Delete(pFrameToFree);

	return S_OK;
}

STDMETHODIMP CHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	for (_ulong i = 0; i < pMeshContainerToFree->NumMaterials; ++i)
		Safe_Release(pMeshContainer->ppTexture[i]);

	Safe_Delete_Array(pMeshContainer->ppTexture);

	Safe_Release(pMeshContainer->pOriMesh);
	Safe_Release(pMeshContainer->MeshData.pMesh);
	Safe_Release(pMeshContainer->pSkinInfo);

	Safe_Delete_Array(pMeshContainer->pFrameOffeSetMatrix);
	Safe_Delete_Array(pMeshContainer->ppFrameCombinedMatrix);
	Safe_Delete_Array(pMeshContainer->pRenderingMatrix);
	Safe_Delete_Array(pMeshContainer->Name);
	Safe_Delete_Array(pMeshContainer->pMaterials);
	Safe_Delete_Array(pMeshContainer->pAdjacency);

	Safe_Delete(pMeshContainer);

	return S_OK;
}

void CHierarchy::Allocate_Name(char ** ppName, const char * pFrameName)
{
	if (nullptr == pFrameName)
		return;

	_uint iLength = strlen(pFrameName);
	*ppName = new char[iLength + 1];

	strcpy_s(*ppName, iLength + 1, pFrameName);
}

CHierarchy * CHierarchy::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pPath)
{
	return new CHierarchy(pGraphicDev, pPath);
}

_ulong CHierarchy::Release(void)
{
	Engine::Safe_Release(m_pGraphicDev);

	delete this;
	return 0;
}
