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
	void Change_PlayerPose();				// 플레이어의 비공격, 공격 상태를 결정해준다.
public:
	virtual HRESULT Ready_Object(const _int& iFlag);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;
	virtual void	Render_Shadow(void) override;
private:			//초기화 함수.
	HRESULT			Add_Component(void);
private:
	void			Key_Input(const _float& fTimeDelta);			// 키 입력 함수.
	void			KeyBoard_Input(const _float& fTimeDelta);			// 플레이어에서 사용하는 키.
	void			KeyMouse_Input(const _float& fTimeDelta);
	void			Public_Key_Input();			// 다른 곳에서도 사용하는 키들을 모아둔곳.
	void			Motion_Event(const _float& fTimeDelta);				// 각 상태에 따른 이벤트들.
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void			MotionChange();
	void			Attack_Landing();			// 공격 모션 다음으로 나오는 착륙 모션을 하기 위함.
	void			Basic_AttackCombo(const _float& fTimeDelta, _double* pEndTime);		// 기본 공격에 관한 콤보.
	void			IsJumping(const _float& fTimeDelta);			// 점프를 했을 시 중력값.
	void			KidneyBlow(const _float& fTimeDelta);			// 스킬1(발차기)
	void			LeapingOver(const _float& fTimeDelta);			// 스킬2(어퍼 2번)
	void			WalkDust(const _float& fTimeDelta);				// 걷고있을때 발자국과 싱크 맞춰서 먼지를 생성.
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
	_bool					m_bIsLanding = false;	// 착륙했는지 안했는지에 대한 판단.
	_float					m_fSpeed;
	_uint					m_iAttStack = 0;		// 콤보공격을 위한 스택.
	_double					m_dEndTime = 0.0;		// EndTime을 기록하기 위함. (기본 콤보공격들의 각각의 끝나는 시간들.)
	_float					m_fAttTimer = 0.f;		// 콤보 공격을 위한 타이머..
	
	_vec3					m_vDir;
	_vec3					m_vMoveDir;			// 구른 직전의 Dir값이다. 이 값으로 움직임을 구르는 움직임에도 이어갈 것이다.
	PLAYER_POSE				m_ePose = ATTACK;

	// 점프
	_bool					m_bIsJump = false;		// 공중에 있는지 없는지.
	_bool					m_bIsJumpOK = false;	// m_bIsJump 변수가 한번만 동작해야한다...
	_float					m_fJumpAccel = 0.0f;
	_float					m_fJumpPower = 0.0f;

	// 스킬
	ATT_MODE				m_eAttMode = BASIC;				// 어택 모드(기본, 스킬1, 스킬2)

	// 걸을때 먼지 생성되는 시간.
	_float					m_fDustTimer = 0.1f;			//  먼지가 생성되기 위한 루프 타임.
	_bool					m_bIsAttEffect = false;

public:
	static CPlayer*			Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh);

private:
	virtual void Free(void) override;
};
#endif // Player_h__
