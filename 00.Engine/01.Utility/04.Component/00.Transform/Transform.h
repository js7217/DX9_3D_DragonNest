#ifndef Transform_h__
#define Transform_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent
{
private:
	explicit CTransform(void);
	virtual ~CTransform(void);

public:
	void Get_Info(INFO eType, _vec3* pInfo);
	const _vec3* Get_Info(INFO eType);
	void Get_WorldMatrix(_matrix* pWorldMatrix) const;
	_matrix* Get_WorldMatrix() { return &m_matWorld; }
	const _vec3& Get_Dir(ROTATION eRot);				// 현재 만들어진 방향을 반환한다.
	const _matrix*	Get_NRotWorldMatrix(_matrix* pWorldMatrix) const;
	const _matrix*	Get_WorldMatrixPointer(void) const { return &m_matWorld; }

public:
	void Set_Position(const _vec3* pPos) { m_vInfo[INFO_POS] = *pPos; }
	void Set_Pos(const _vec3* const pDir);
	void Set_Pos(const _float& fX, const _float& fY, const _float& fZ);
	void Set_Scale(const _float& fX, const _float& fY, const _float& fZ);
	void Rotation(ROTATION eType, const _float& fAngle);
	void Move_TargetPos(const _vec3* pTargetPos, const _float& fSpeed, const _float& fTimeDelta);
	void Set_ParentMatrix(const _matrix& matParent);
public:
	void			Chase_Target(const _vec3* pTargetPos, const _float& fSpeed);
	const _matrix*	Compute_LookAtTarget(const _vec3* pTargetPos);
	void			Chase_Target_RotY(const _vec3& vTargetPos, ROTATION eRot);
public:
	HRESULT			Ready_Transform(const _vec3& vLook);
	virtual _int	Update_Component(const _float& fTimeDelta);

public:
	_vec3		m_vDir; // 방향
	_vec3		m_vInfo[INFO_END];
	_vec3		m_vScale;
	_vec3		m_vAngle;
	_matrix		m_matWorld;
	_matrix		m_matNRotWorld;
	_vec3		m_vLook;

public:
	static CTransform*		Create(const _vec3& vLook = _vec3(0.f, 0.f, 1.f));

private:
	virtual void			Free(void);
};

END
#endif // Transform_h__
