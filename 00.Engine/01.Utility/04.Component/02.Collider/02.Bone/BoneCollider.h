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
	_float				m_fRadius;				// ������(����)

	LPDIRECT3DDEVICE9	m_pGraphicDev;
	LPD3DXMESH			m_pSphere;
	D3DMATERIAL9		m_mtrl;
	map<const char*, const _matrix*> m_mapFrame;		// �ش� ���� ��� �����͸� ���������� ��.(������)
	_matrix*			m_pmatTargetWorld = nullptr;	// �� ������Ʈ�� ����ϴ� ��ü�� ��������� ������.
public:
	static CBoneCollider*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void Free();
};
END
#endif // BoneCollider_h__
