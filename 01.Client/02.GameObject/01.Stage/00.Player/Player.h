#ifndef Player_h__
#define Player_h__

#include "Defines.h"
#include "GameObject.h"
#include "Cell.h"
namespace Engine
{
	class CCalculator;
	class CTransform;
	class CRenderer;
	class CDynamicMesh;
	class CNaviMesh;
	class CShader;
	class CBoneCollider;
	class CKeyMgr;
}

class CPlayer : public Engine::CGameObject
{
public:
	enum PLAYER_POSE { ATTACK, NORMAL = 4 };
	enum ATT_MODE { BASIC, KIDNEYBLOW, LEAPINGOVER };
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer(void);

public:
	Engine::CTransform* Get_TransformCom();
	ATT_MODE Get_AttMode() { return m_eAttMode; }
public:
	void Set_NaviCom(Engine::CNaviMesh* pNaviMesh);
	void Set_BossStage();
	void Change_PlayerPose();				// �÷��̾��� �����, ���� ���¸� �������ش�.
public:
	virtual HRESULT Ready_Object(const _int& iFlag);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;
	virtual void	Render_Shadow(void) override;
private:			//�ʱ�ȭ �Լ�.
	HRESULT			Add_Component(void);
private:
	void			Key_Input(const _float& fTimeDelta);			// Ű �Է� �Լ�.
	void			KeyBoard_Input(const _float& fTimeDelta);			// �÷��̾�� ����ϴ� Ű.
	void			KeyMouse_Input(const _float& fTimeDelta);
	void			Public_Key_Input();			// �ٸ� �������� ����ϴ� Ű���� ��Ƶа�.
	void			Motion_Event(const _float& fTimeDelta);				// �� ���¿� ���� �̺�Ʈ��.
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void			MotionChange();
	void			Attack_Landing();			// ���� ��� �������� ������ ���� ����� �ϱ� ����.
	void			Basic_AttackCombo(const _float& fTimeDelta, _double* pEndTime);		// �⺻ ���ݿ� ���� �޺�.
	void			IsJumping(const _float& fTimeDelta);			// ������ ���� �� �߷°�.
	void			KidneyBlow(const _float& fTimeDelta);			// ��ų1(������)
	void			LeapingOver(const _float& fTimeDelta);			// ��ų2(���� 2��)
	void			WalkDust(const _float& fTimeDelta);				// �Ȱ������� ���ڱ��� ��ũ ���缭 ������ ����.
public:
	void		Create_Cell(const _vec3& vPos, const Engine::CCell::CELL_OPTION& eOption);

private:
	Engine::CTransform*		m_pTransCom;
	Engine::CRenderer*		m_pRenderCom;
	Engine::CCalculator*	m_pCalculatorCom;
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CNaviMesh*		m_pNaviMeshCom;
	Engine::CShader*		m_pShaderCom;
	Engine::CBoneCollider*	m_pBoneColliderCom;

	Engine::CKeyMgr*		m_pKeyMgr;

	_bool					m_bIsColl;
	_bool					m_bIsLanding = false;	// �����ߴ��� ���ߴ����� ���� �Ǵ�.
	_float					m_fSpeed;
	_uint					m_iAttStack = 0;		// �޺������� ���� ����.
	_double					m_dEndTime = 0.0;		// EndTime�� ����ϱ� ����. (�⺻ �޺����ݵ��� ������ ������ �ð���.)
	_float					m_fAttTimer = 0.f;		// �޺� ������ ���� Ÿ�̸�..
	
	_vec3					m_vDir;
	_vec3					m_vMoveDir;			// ���� ������ Dir���̴�. �� ������ �������� ������ �����ӿ��� �̾ ���̴�.
	PLAYER_POSE				m_ePose = ATTACK;

	// ����
	_bool					m_bIsJump = false;		// ���߿� �ִ��� ������.
	_bool					m_bIsJumpOK = false;	// m_bIsJump ������ �ѹ��� �����ؾ��Ѵ�...
	_float					m_fJumpAccel = 0.0f;
	_float					m_fJumpPower = 0.0f;

	// ��ų
	ATT_MODE				m_eAttMode = BASIC;				// ���� ���(�⺻, ��ų1, ��ų2)

	// ������ ���� �����Ǵ� �ð�.
	_float					m_fDustTimer = 0.1f;			//  ������ �����Ǳ� ���� ���� Ÿ��.
	_bool					m_bIsAttEffect = false;

public:
	static CPlayer*			Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh);

private:
	virtual void Free(void) override;
};
#endif // Player_h__
