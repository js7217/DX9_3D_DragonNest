#ifndef BoxCollider_h__
#define BoxCollider_h__

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CBoxCollider : public CComponent
{
private:
	explicit	CBoxCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		~CBoxCollider(void);

public:
	const _vec3*	Get_Min(void) { return &m_vMin; }
	const _vec3*	Get_Max(void) { return &m_vMax; }
	const _matrix*	Get_ColWorld(void) { return &m_matColWorld; }

public:
	HRESULT		Ready_Collider(const _vec3* pPos, const _ulong& dwNumVtx, const _ulong& dwStride);
	void		Render_Collider(COLLTYPE eType, const _matrix* pColliderMatrix);

private:
	_vec3				m_vMin, m_vMax;
	_matrix				m_matColWorld;

	LPDIRECT3DDEVICE9		m_pGraphicDev;
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	LPDIRECT3DTEXTURE9		m_pTexture[COL_END];

public:
	static CBoxCollider*	Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _vec3* pPos,
		const _ulong& dwNumVtx,
		const _ulong& dwStride);
	virtual void Free();
};
END
#endif // BoxCollider_h__
