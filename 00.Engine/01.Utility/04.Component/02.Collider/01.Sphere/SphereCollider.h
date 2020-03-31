#ifndef SphereCollider_h__
#define SphereCollider_h__

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CSphereCollider : public CComponent
{
private:
	explicit	CSphereCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		~CSphereCollider(void);

public:
	HRESULT		Ready_Collider(const LPD3DXMESH& pMesh);
	void		Render_Collider(const _matrix* pColliderMatrix);

private:
	_vec3				m_vCenter;
	_float				m_fRadius;
	_matrix				m_matColWorld;

	LPDIRECT3DDEVICE9	m_pGraphicDev;
	LPD3DXMESH			m_pSphere;

public:
	static CSphereCollider*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const LPD3DXMESH& pMesh);
	virtual void Free();
};
END
#endif // SphereCollider_h__
