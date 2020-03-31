#ifndef StaticMesh_h__
#define StaticMesh_h__

#include "Mesh.h"

BEGIN(Engine)

class ENGINE_DLL CStaticMesh : public CMesh
{
private:
	explicit CStaticMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CStaticMesh(const CStaticMesh& rhs);
	virtual ~CStaticMesh(void);

public:
	const LPD3DXMESH&	Get_Mesh() { return m_pMesh; }
	_ulong				Get_NumVtx(void) const { return m_dwNumVtx; }
	_ulong				Get_Stride(void) const { return m_dwStride; }
	const _vec3*		Get_VtxMeshPos(void) const { return m_pVtxPos; }

public:
	HRESULT	Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName);
	void	Render_Meshes(void);
	void	Render_Meshes(LPD3DXEFFECT pEffect);

	_bool	Find_Alpha(const char* pFileName);

private:
	LPD3DXMESH				m_pOriMesh; // �ϴ� ���� �޽��� ����� ���� �޽��� ����� ���°�� ���������ϱ� ������ �ʱ⺻���� �޽��� ���´�.
	LPD3DXMESH				m_pMesh;
	LPD3DXBUFFER			m_pAdjacency;		// ������ ������ �����ϱ� ���� �ڷ���
	LPD3DXBUFFER			m_pSubsetBuffer;
	D3DXMATERIAL*			m_pSubsets;
	_ulong					m_dwSubsetCnt;

	LPDIRECT3DTEXTURE9*		m_ppTextures;

	_ulong					m_dwNumVtx; // �޽� ���ؽ� ������ �����ϱ� ����.
	_ulong					m_dwStride; // �̿��ϴ� �������� ����
	_vec3*					m_pVtxPos; // �������� ��ġ�� �˾ƾ� �ε����� ��ų� �� �� ����.

public:
	static CStaticMesh*	Create(LPDIRECT3DDEVICE9 pGraphicDev, 
								const _tchar* pFilePath, 
								const _tchar* pFileName);
	virtual CResources*	Clone(void);
	virtual void		Free(void);

};

END
#endif // StaticMesh_h__
