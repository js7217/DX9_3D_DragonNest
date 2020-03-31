#ifndef DynamicMesh_h__
#define DynamicMesh_h__

#include "Mesh.h"
#include "Hierarchy.h"
#include "AniCtrl.h"

BEGIN(Engine)

class ENGINE_DLL CDynamicMesh : public CMesh
{
private:
	explicit CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicMesh(const CDynamicMesh& rhs);
	virtual ~CDynamicMesh(void);

public:
	const	D3DXFRAME_DERIVED*	Get_FrameByName(const char* pFrameName);
	_bool						Is_AnimationSetEnd(_double EndTime = 0.1);
	_bool						Is_AnimationAfterTrack(_double AfterTime);
	const _uint&				Get_AnimationNum();
	_double						Get_CurAnimationPosition();
	const list<D3DXMESHCONTAINER_DERIVED*>& Get_MeshContainerList();

public:
	HRESULT Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName);
	void	Render_Meshes(void);
	void	Render_Meshes(LPD3DXEFFECT pEffect);

	void	Set_AnimationSet(const _uint& iIndex);
	void	Play_AnimationSet(const _float& fTimeDelta);

private:
	//			모든 뼈들을 순회하면서 뼈들이 가지고 있는 행렬(CombinedTransformMatrix)을 갱신하는 함수
	void Update_FrameMatrix(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentMatrix);
	void SetUp_FrameMatrixPointer(D3DXFRAME_DERIVED* pFrame);

private:
	D3DXFRAME*			m_pRootFrame;
	CHierarchy*			m_pLoader;
	CAniCtrl*			m_pAniCtrl;

	list<D3DXMESHCONTAINER_DERIVED*>	m_MeshContainerList;
public:
	static CDynamicMesh* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _tchar* pFilePath, const _tchar* pFileName);

	virtual CResources* Clone(void);
	virtual void		Free(void);
};
END
#endif // DynamicMesh_h__
//typedef struct _D3DXFRAME
//{
//	LPSTR                   Name;					// 뼈의 이름을 저장
//	D3DXMATRIX              TransformationMatrix;	
// 뼈들이 상당히 많은데 로드한 뼈의 행렬(중요뼈들, 중심이 되는 뼈)
// -> 행렬의 포지션 값은 중심이 되는 뼈를 기준(0,0,0)으로 포지션 값이 들어간다. (월드기준)
//	LPD3DXMESHCONTAINER     pMeshContainer;
// 사용하는 메쉬 형태는 Skined Mesh이다. 의미적으로 살과 같은 의미, 메쉬파일, 읽어들여지는 파일이 있을수도 있고 없을수도 있다.
// -> 원소 메쉬의 정보를 저장하는 구조체.(모든 메쉬 컨테이너들의 뼈 정보를 갖고 있는건 아니다.)
//	struct _D3DXFRAME       *pFrameSibling;		// 형제 뼈의 주소 ex) 걸어갈때의 팔과 다리같은 느낌.
//	struct _D3DXFRAME       *pFrameFirstChild;	// 자식 뼈의 주소(시작 뼈대의 주소를 알면 나머지 뼈대들을 순회할 수 있게끔 설계하고자 존재하는 포인터)
// ex) 시작뼈대 주소를 알면 시작을 기준으로 순회하며 통제가 가능하다. 팔과 팔목 느낌
//} D3DXFRAME, *LPD3DXFRAME;
//
//typedef struct _D3DXMESHCONTAINER
//{
//	LPSTR                   Name;			// 메쉬 컨테이너의 이름(거의 없음)
// -> 뼈는 중요하므로 있을 가능성이 높지만 메쉬는 거의 없음.
//	D3DXMESHDATA            MeshData;		// 메쉬 컴객체를 저장한 구조체 자료형
//
//	LPD3DXMATERIAL          pMaterials;		// 메쉬의 재질 정보를 담고 있는 객체(조명연산 시 사용했던 재질 정보와는 달리 동적 배열로 서브셋의 개수만큼 할당하여 값을 채워 넣어줄 예정)
//	LPD3DXEFFECTINSTANCE    pEffects;		// 메쉬에 내부적으로 탑재된 여러 리소스 정보, 우리는 사용하지 않으므로 null을 넣을 예정
//	DWORD                   NumMaterials;	// 재질의 개수(서브셋의 개수)
//	DWORD                  *pAdjacency;		// 인접한 폴리곤의 정보를 저장하기 위한 포인터
// -> 이걸 알아야 보간이 가능하다.
//	LPD3DXSKININFO          pSkinInfo;		// 뼈가 아닌 메쉬와 관련된 각종 정보를 저장하는 객체 포인터(실제적으로 메쉬 컨테이너에 영향을 주는 각종 뼈에 대한 정보를 제공하는 객체)
// -> 뼈대의 개수와 같은 정보를 함수를 통해 얻어올 수 있다.
//	struct _D3DXMESHCONTAINER *pNextMeshContainer;	// 다음 메쉬 컨테이너의 주소를 저장하는 포인터, 하지만 일반적으로 이 값을 저장하지 않는다, 이유는 뼈들이 이미 계층 구조화 되어있어서 메쉬 컨테이너 간의 상관 관계를 성립할 필요가 없기 때문이다.
//} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;