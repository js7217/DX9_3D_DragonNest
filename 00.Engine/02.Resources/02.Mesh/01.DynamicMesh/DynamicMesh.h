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
	//			��� ������ ��ȸ�ϸ鼭 ������ ������ �ִ� ���(CombinedTransformMatrix)�� �����ϴ� �Լ�
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
//	LPSTR                   Name;					// ���� �̸��� ����
//	D3DXMATRIX              TransformationMatrix;	
// ������ ����� ������ �ε��� ���� ���(�߿����, �߽��� �Ǵ� ��)
// -> ����� ������ ���� �߽��� �Ǵ� ���� ����(0,0,0)���� ������ ���� ����. (�������)
//	LPD3DXMESHCONTAINER     pMeshContainer;
// ����ϴ� �޽� ���´� Skined Mesh�̴�. �ǹ������� ��� ���� �ǹ�, �޽�����, �о�鿩���� ������ �������� �ְ� �������� �ִ�.
// -> ���� �޽��� ������ �����ϴ� ����ü.(��� �޽� �����̳ʵ��� �� ������ ���� �ִ°� �ƴϴ�.)
//	struct _D3DXFRAME       *pFrameSibling;		// ���� ���� �ּ� ex) �ɾ���� �Ȱ� �ٸ����� ����.
//	struct _D3DXFRAME       *pFrameFirstChild;	// �ڽ� ���� �ּ�(���� ������ �ּҸ� �˸� ������ ������� ��ȸ�� �� �ְԲ� �����ϰ��� �����ϴ� ������)
// ex) ���ۻ��� �ּҸ� �˸� ������ �������� ��ȸ�ϸ� ������ �����ϴ�. �Ȱ� �ȸ� ����
//} D3DXFRAME, *LPD3DXFRAME;
//
//typedef struct _D3DXMESHCONTAINER
//{
//	LPSTR                   Name;			// �޽� �����̳��� �̸�(���� ����)
// -> ���� �߿��ϹǷ� ���� ���ɼ��� ������ �޽��� ���� ����.
//	D3DXMESHDATA            MeshData;		// �޽� �İ�ü�� ������ ����ü �ڷ���
//
//	LPD3DXMATERIAL          pMaterials;		// �޽��� ���� ������ ��� �ִ� ��ü(������ �� ����ߴ� ���� �����ʹ� �޸� ���� �迭�� ������� ������ŭ �Ҵ��Ͽ� ���� ä�� �־��� ����)
//	LPD3DXEFFECTINSTANCE    pEffects;		// �޽��� ���������� ž��� ���� ���ҽ� ����, �츮�� ������� �����Ƿ� null�� ���� ����
//	DWORD                   NumMaterials;	// ������ ����(������� ����)
//	DWORD                  *pAdjacency;		// ������ �������� ������ �����ϱ� ���� ������
// -> �̰� �˾ƾ� ������ �����ϴ�.
//	LPD3DXSKININFO          pSkinInfo;		// ���� �ƴ� �޽��� ���õ� ���� ������ �����ϴ� ��ü ������(���������� �޽� �����̳ʿ� ������ �ִ� ���� ���� ���� ������ �����ϴ� ��ü)
// -> ������ ������ ���� ������ �Լ��� ���� ���� �� �ִ�.
//	struct _D3DXMESHCONTAINER *pNextMeshContainer;	// ���� �޽� �����̳��� �ּҸ� �����ϴ� ������, ������ �Ϲ������� �� ���� �������� �ʴ´�, ������ ������ �̹� ���� ����ȭ �Ǿ��־ �޽� �����̳� ���� ��� ���踦 ������ �ʿ䰡 ���� �����̴�.
//} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;