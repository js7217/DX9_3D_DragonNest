#ifndef GameObject_h__
#define GameObject_h__

#include "Engine_Defines.h"
#include "Base.h"

#include "Component.h"
BEGIN(Engine)

class ENGINE_DLL CGameObject : public CBase
{
public:
	enum STATE { STAND, WALK, ATTACK, DAMAGE, JUMP, DIE };
	enum HIT_OPTION { HIT_NORMAL, HIT_KNOCKBACK };
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGameObject(void);

public:
	CComponent*			Get_Component(const _tchar* ComponentKey, COMPONENTID eID);
	_float				Get_ViewZ(void) const { return m_fViewZ; }
	const STATE&		Get_State() { return m_eCurState; }
	const HIT_OPTION&	Get_HitOption() { return m_eHitOption; }
	const _bool&		Get_Hit() { return m_bIsHit; }
	const _int&			Get_CurHp() { return m_iHp; }
	const _uint&		Get_Group() { return m_iGroup; }


public:
	void			Set_State(const STATE& eState) { m_eCurState = eState; }
	void			Set_Hit();// 요거 데미지 줘야함. m_iHp--; m_bIsHit = true; 
	void			Reset_Hit() { m_bIsHit = false; }
	void			Set_HitOption(const HIT_OPTION& eHitOption = HIT_NORMAL) { m_eHitOption = eHitOption; }
	void			Set_Group(const _uint& iGroup) { m_iGroup = iGroup; }
	void			Set_Open() { m_bIsOpen = true; }			// 오픈시킨다. 
public:
	virtual HRESULT Ready_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta);
	virtual void	Render_Object(void);
	virtual void	Render_Shadow(void);

public:
	virtual void	Set_HitEffect() {};						// 맞았을때 이펙트를 터트리기 위함. 상속받아 사용함.
public:
	void			Compute_ViewZ(const _vec3* pPos);
	HRESULT			Delete_Component(const _tchar* ComponentKey, COMPONENTID eID);
	_bool			CollisionSphere(const _matrix& matSphere, const _float& fTargetRadius);
private:
	CComponent* Find_Component(const _tchar* ComponentKey, COMPONENTID eID);
protected:
	LPDIRECT3DDEVICE9				m_pGraphicDev;
	map<const _tchar*, CComponent*>	m_mapComponent[ID_END];
	_float							m_fViewZ = 0.f;
	
	_matrix					m_matBone;					// 구의 위치값을 위한 행렬.
	_float					m_fRadius = 0.f;			// 구의 반지름.

	STATE					m_eCurState;				// 현재 상태
	STATE					m_ePreState;				// 이전 상태
	const char*				m_pBoneName = nullptr;		// 뼈 이름

	// 충돌(맞았을때를 위함)
	_bool					m_bIsHit = false;		// 해당 객체가 공격을 했는지 안했는지 판단하기 위함. 즉 할때 

	// 맞기 전 Hit 되었는지 안되었는지 판단하고 때린다. 그리고 나서 맞았을때 Hit되었다고 하고 플레이어가 애니메이션 동작을 바꾸거나 여러대 맞아야하는 공격이라면
	// 몬스터들의 Hit 값을 다시 되돌려버린다?
	// 스테이터스
	_int					m_iHp;
	// 맞았을 때의옵션.
	HIT_OPTION				m_eHitOption = HIT_NORMAL;
	// 던전마다의 그룹
	_uint					m_iGroup = 100;
	// 문이 열릴 시에 업데이트와 렌더가 돌아야한다.
	_bool					m_bIsOpen = false;
public: 
	virtual void Free(void);
};

END
#endif // GameObject_h__
