#ifndef BoneCollider_h__
#define BoneCollider_h__

#include "Component.h"
#include "DynamicMesh.h"

BEGIN(Engine)
class ENGINE_DLL CBoneCollider : public CComponent
{
private:
	explicit	CBoneCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		~CBoneCollider(void);

public:
	const _float& Get_Radius() { return m_fRadius; }
public:
	void		Set_BoneName(const char* pBoneName, Engine::CDynamicMesh * pMeshCom, _matrix* pmatTargetWorld);
	void		Set_Radius(const _vec3& vScale);
public:
	HRESULT		Ready_Collider();
	_matrix		Current_BoneMatrix(const char * pBoneName);
	void		Render_Collider();
	
private:
	_vec3				m_vPos;					// Center Pos
	_float				m_fRadius;				// 반지름(월드)

	LPDIRECT3DDEVICE9	m_pGraphicDev;
	LPD3DXMESH			m_pSphere;
	D3DMATERIAL9		m_mtrl;
	map<const char*, const _matrix*> m_mapFrame;		// 해당 뼈의 행렬 포인터를 가지고있을 것.(원본값)
	_matrix*			m_pmatTargetWorld = nullptr;	// 이 컴포넌트를 사용하는 객체의 월드행렬의 포인터.
public:
	static CBoneCollider*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void Free();
};
END
#endif // BoneCollider_h__
