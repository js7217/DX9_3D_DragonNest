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
	void			Set_Hit();// ��� ������ �����. m_iHp--; m_bIsHit = true; 
	void			Reset_Hit() { m_bIsHit = false; }
	void			Set_HitOption(const HIT_OPTION& eHitOption = HIT_NORMAL) { m_eHitOption = eHitOption; }
	void			Set_Group(const _uint& iGroup) { m_iGroup = iGroup; }
	void			Set_Open() { m_bIsOpen = true; }			// ���½�Ų��. 
public:
	virtual HRESULT Ready_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta);
	virtual void	Render_Object(void);
	virtual void	Render_Shadow(void);

public:
	virtual void	Set_HitEffect() {};						// �¾����� ����Ʈ�� ��Ʈ���� ����. ��ӹ޾� �����.
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
	
	_matrix					m_matBone;					// ���� ��ġ���� ���� ���.
	_float					m_fRadius = 0.f;			// ���� ������.

	STATE					m_eCurState;				// ���� ����
	STATE					m_ePreState;				// ���� ����
	const char*				m_pBoneName = nullptr;		// �� �̸�

	// �浹(�¾������� ����)
	_bool					m_bIsHit = false;		// �ش� ��ü�� ������ �ߴ��� ���ߴ��� �Ǵ��ϱ� ����. �� �Ҷ� 

	// �±� �� Hit �Ǿ����� �ȵǾ����� �Ǵ��ϰ� ������. �׸��� ���� �¾����� Hit�Ǿ��ٰ� �ϰ� �÷��̾ �ִϸ��̼� ������ �ٲٰų� ������ �¾ƾ��ϴ� �����̶��
	// ���͵��� Hit ���� �ٽ� �ǵ���������?
	// �������ͽ�
	_int					m_iHp;
	// �¾��� ���ǿɼ�.
	HIT_OPTION				m_eHitOption = HIT_NORMAL;
	// ���������� �׷�
	_uint					m_iGroup = 100;
	// ���� ���� �ÿ� ������Ʈ�� ������ ���ƾ��Ѵ�.
	_bool					m_bIsOpen = false;
public: 
	virtual void Free(void);
};

END
#endif // GameObject_h__
