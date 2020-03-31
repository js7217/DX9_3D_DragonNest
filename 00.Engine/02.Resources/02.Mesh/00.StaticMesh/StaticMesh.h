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
	LPD3DXMESH				m_pOriMesh; // 일단 원본 메쉬를 만들고 만약 메쉬에 노멀이 없는경우 만들어줘야하기 때문에 초기본으로 메쉬를 갖는다.
	LPD3DXMESH				m_pMesh;
	LPD3DXBUFFER			m_pAdjacency;		// 인접한 정보를 저장하기 위한 자료형
	LPD3DXBUFFER			m_pSubsetBuffer;
	D3DXMATERIAL*			m_pSubsets;
	_ulong					m_dwSubsetCnt;

	LPDIRECT3DTEXTURE9*		m_ppTextures;

	_ulong					m_dwNumVtx; // 메쉬 버텍스 개수를 저장하기 위함.
	_ulong					m_dwStride; // 이웃하는 폴리곤의 개수
	_vec3*					m_pVtxPos; // 정점들의 위치를 알아야 인덱스를 잡거나 할 수 있음.

public:
	static CStaticMesh*	Create(LPDIRECT3DDEVICE9 pGraphicDev, 
								const _tchar* pFilePath, 
								const _tchar* pFileName);
	virtual CResources*	Clone(void);
	virtual void		Free(void);

};

END
#endif // StaticMesh_h__
