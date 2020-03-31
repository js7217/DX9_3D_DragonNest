#include "stdafx.h"
#include "Player.h"

#include "Export_Function.h"
#include "Dust.h"
#include "HitEffect.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_vDir(0.f, 0.f, 0.f), m_vMoveDir(0.f, 0.f, 0.f)
	, m_bIsColl(false)
	, m_pKeyMgr(Engine::Get_KeyMgr())
{

}

CPlayer::~CPlayer(void)
{

}

Engine::CTransform * CPlayer::Get_TransformCom()
{
	return m_pTransCom;
}

void CPlayer::Set_NaviCom(Engine::CNaviMesh * pNaviMesh)
{
	_int iIndex = 0;
	m_pNaviMeshCom->Get_PickCell(m_pTransCom->m_vInfo[Engine::INFO_POS], &iIndex);
	m_pNaviMeshCom->SetCurrentIndex(iIndex);
}

void CPlayer::Set_BossStage()
{
	m_pNaviMeshCom->SetCurrentIndex(0);
	m_pTransCom->Set_Pos(4.2f, 0.0f, 1.1f);
}

void CPlayer::Change_PlayerPose()
{
	switch (m_ePose)
	{
	case CPlayer::ATTACK:
		m_ePose = NORMAL;
		break;
	case CPlayer::NORMAL:
		m_ePose = ATTACK;
		break;
	default:
		break;
	}
}

HRESULT CPlayer::Ready_Object(const _int& iFlag)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fSpeed = 1.f;

	m_pMeshCom->Set_AnimationSet(18);

	m_pTransCom->Set_Scale(0.0001f, 0.0001f, 0.0001f);
	m_pTransCom->Set_Pos(9.5f, 0.0f, 2.f); // 던전
//	m_pTransCom->Set_Pos(4.2f, 0.0f, 1.1f);
	// Collider
	m_pBoneColliderCom->Set_BoneName("_Main", m_pMeshCom, &(m_pTransCom->m_matWorld));
	m_pBoneColliderCom->Set_BoneName("Bip01_Spine", m_pMeshCom, &(m_pTransCom->m_matWorld));
	m_pBoneColliderCom->Set_BoneName("Bip01_R_Toe0", m_pMeshCom, &(m_pTransCom->m_matWorld));

	m_pBoneColliderCom->Set_Radius(m_pTransCom->m_vScale);


	m_fRadius = m_pBoneColliderCom->Get_Radius();				// 만들어진 반지름을 가져온다.
	m_pBoneName = "Bip01_Spine";
	m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);		// 현재 검사하고 싶은 뼈의 행렬.

	return S_OK;
}

_int CPlayer::Update_Object(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);
	Engine::CGameObject::Update_Object(fTimeDelta);
//	Set_OnTerrain();
	MotionChange();					//모션 바꾸기
	Motion_Event(fTimeDelta);		// 모션마다의 이벤트 처리;.

	// 네비를 태우는 pos 값을 모두 구한 뒤 y좌표 값을 새로이 구해준다.
	IsJumping(fTimeDelta);
	m_pMeshCom->Play_AnimationSet(fTimeDelta * 2.f);


//	m_bIsColl = Collision_ToObject(L"GameLogic", L"Stone");

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_SHADOW, this);
	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	return 0;
}

void CPlayer::Render_Object(void)
{
	m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);

	if(GetAsyncKeyState(VK_TAB) & 0x8000)
		m_pNaviMeshCom->Render_NaviMesh();

	/*_matrix matWorld;
	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/

	LPD3DXEFFECT		pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	m_pMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	// Collider
#ifdef _DEBUG
	m_pBoneColliderCom->Render_Collider();
#endif
}

void CPlayer::Render_Shadow(void)
{
	LPD3DXEFFECT		pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();

	_matrix		matView, matProj;

	_vec3 vEye = { 9.5f, 0.1f, 0.f };
	_vec3 vAt = { 9.5f, 0.05f, 5.f };

	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &_vec3(0.f, 1.f, 0.f));
	D3DXMatrixPerspectiveFovLH(&matProj, 90.f, _float(WINCX) / WINCY,0.1f, 500.f);

	pEffect->SetMatrix("g_matWorld", &m_pTransCom->m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(2);

	m_pMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CPlayer::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);
	
	pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_CalculatorCom", pComponent);

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_Machina"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// NaviMesh
	pComponent = m_pNaviMeshCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Clone(L"Proto_NaviMesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);

	// Navi Com
	//pComponent = m_pNaviMeshCom = dynamic_cast<Engine::CNaviMesh*>
	//	(Engine::Clone(L"Proto_NaviMesh"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);

	// NaviMesh
	//pComponent = m_pNaviMeshCom = dynamic_cast<Engine::CNaviMesh*>
	//	(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_Navi"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	// Bone Collider
	pComponent = m_pBoneColliderCom = Engine::CBoneCollider::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_BoneCollider", pComponent);
	return S_OK;
}

void CPlayer::Key_Input(const _float & fTimeDelta)
{
	Public_Key_Input();

	switch (m_eCurState)
	{
	case Engine::CGameObject::DAMAGE:
		if (m_eHitOption == CGameObject::HIT_KNOCKBACK)
			return;
	case Engine::CGameObject::STAND:
	case Engine::CGameObject::WALK:
	case Engine::CGameObject::DIE:
		KeyBoard_Input(fTimeDelta);
		break;
	case Engine::CGameObject::ATTACK:
		KeyMouse_Input(fTimeDelta);
		switch (m_eAttMode)
		{
		case CPlayer::KIDNEYBLOW:
			KidneyBlow(fTimeDelta);
			break;
		case CPlayer::LEAPINGOVER:
			LeapingOver(fTimeDelta);
			break;
		}
		break;
		//KeyMouse_Input(fTimeDelta);
		//break;
	}

	//if (m_eCurState != Engine::CGameObject::JUMP ||
	//	m_eCurState != Engine::CGameObject::ATTACK)			// 점프 상태일때는 점프하는 상태의 움직임을 이어간다.(MotionChange에서 수행함).
	//	KeyBoard_Input(fTimeDelta);
	
}

void CPlayer::KeyBoard_Input(const _float & fTimeDelta)
{
	_vec3 vDir = {};
	ZeroMemory(&m_vMoveDir, sizeof(_vec3));
	ZeroMemory(&vDir, sizeof(_vec3));

	m_pTransCom->Get_Info(Engine::INFO_LOOK, &m_vDir);
	m_vDir *= 1000.f;
	D3DXVec3Normalize(&m_vDir, &m_vDir);

	_vec3 vUp = { 0.f, 1.f, 0.f };
	_vec3 vRight = {};
	D3DXVec3Cross(&vRight, &vUp, &m_vDir);
	D3DXVec3Normalize(&vRight, &vRight);			// 회전함에 따라 vRight 벡터 재정의.

	_vec3 vPos = {};								// NaviMesh에 대한 처ㅏ리.

	if (m_pKeyMgr->KeyPressing(KEY_A))
	{
		m_vMoveDir = -vRight;
		vDir = -vRight;

		m_eCurState = Engine::CGameObject::WALK;
		m_pMeshCom->Set_AnimationSet(20 + m_ePose);

		if (m_pKeyMgr->KeyPressing(KEY_W))
			vDir += m_vDir;
		else if (m_pKeyMgr->KeyPressing(KEY_S))
			vDir += -m_vDir;

		if (m_pKeyMgr->KeyDown(KEY_SPACE))
		{
			m_eCurState = Engine::CGameObject::JUMP;
			m_pMeshCom->Set_AnimationSet(6);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);
			CSoundMgr::GetInstance()->MyPlaySound(L"machina_motion_jump_01.ogg", CSoundMgr::PLAYER);
		}

		D3DXVec3Normalize(&vDir, &vDir);
		vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * fTimeDelta * m_fSpeed), fTimeDelta, m_fSpeed);
		m_pTransCom->Set_Position(&vPos);
	}
	else if (m_pKeyMgr->KeyPressing(KEY_D))
	{
		m_vMoveDir = vRight;
		vDir = vRight;

		m_eCurState = Engine::CGameObject::WALK;
		m_pMeshCom->Set_AnimationSet(19 + m_ePose);

		if (m_pKeyMgr->KeyPressing(KEY_W))
			vDir += m_vDir;
		else if (m_pKeyMgr->KeyPressing(KEY_S))
			vDir += -m_vDir;

		if (m_pKeyMgr->KeyDown(KEY_SPACE))
		{
			m_eCurState = Engine::CGameObject::JUMP;
			m_pMeshCom->Set_AnimationSet(5);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);
			CSoundMgr::GetInstance()->MyPlaySound(L"machina_motion_jump_01.ogg", CSoundMgr::PLAYER);
		}

		D3DXVec3Normalize(&vDir, &vDir);
		vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * fTimeDelta * m_fSpeed), fTimeDelta, m_fSpeed);
		m_pTransCom->Set_Position(&vPos);

	}
	else if (m_pKeyMgr->KeyPressing(KEY_W))
	{
		m_vMoveDir = m_vDir;
		vDir = m_vDir; // 움직이는 방향 설정.
		m_eCurState = Engine::CGameObject::WALK;
		m_pMeshCom->Set_AnimationSet(22 + m_ePose);

		if (m_pKeyMgr->KeyDown(KEY_SPACE))
		{
			m_eCurState = Engine::CGameObject::JUMP;
			m_pMeshCom->Set_AnimationSet(7);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);
			CSoundMgr::GetInstance()->MyPlaySound(L"machina_motion_jump_01.ogg", CSoundMgr::PLAYER);
		}

		D3DXVec3Normalize(&vDir, &vDir);
		vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * fTimeDelta * m_fSpeed), fTimeDelta, m_fSpeed);
		m_pTransCom->Set_Position(&vPos);
	}
	else if (m_pKeyMgr->KeyPressing(KEY_S))
	{
		m_vMoveDir = -m_vDir;
		vDir = -m_vDir;
		m_eCurState = Engine::CGameObject::WALK;
		m_pMeshCom->Set_AnimationSet(21 + m_ePose);

		if (m_pKeyMgr->KeyDown(KEY_SPACE))
		{
			m_eCurState = Engine::CGameObject::JUMP;
			m_pMeshCom->Set_AnimationSet(8);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);
			CSoundMgr::GetInstance()->MyPlaySound(L"machina_motion_jump_01.ogg", CSoundMgr::PLAYER);
		}

		D3DXVec3Normalize(&vDir, &vDir);
		vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * fTimeDelta * m_fSpeed), fTimeDelta, m_fSpeed);
		m_pTransCom->Set_Position(&vPos);
	}
	else
	{
		switch (m_eCurState)
		{
		case Engine::CGameObject::STAND:
		case Engine::CGameObject::WALK:
		case Engine::CGameObject::JUMP:
			m_eCurState = Engine::CGameObject::STAND;
			break;
		case Engine::CGameObject::DIE:
			break;
		case Engine::CGameObject::DAMAGE:
			break;
		default:
			break;
		}
	}

	if (m_pKeyMgr->KeyDown(KEY_LBUTTON))
		m_eCurState = CGameObject::ATTACK;
	if (GetAsyncKeyState('1') & 0x8000)			// 스킬 버튼으로 처리할것들.
	{
		m_eCurState = CGameObject::ATTACK;
		m_pMeshCom->Set_AnimationSet(10);
		m_eAttMode = KIDNEYBLOW;
	}
	if (GetAsyncKeyState('2') & 0x8000)			// 스킬 버튼으로 처리할것들.
	{
		m_eCurState = CGameObject::ATTACK;
		m_pMeshCom->Set_AnimationSet(1);
		m_eAttMode = LEAPINGOVER;
	}

	//D3DXVec3Normalize(&vDir, &vDir);
	//vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * fTimeDelta * m_fSpeed), fTimeDelta, m_fSpeed);
	//m_pTransCom->Set_Position(&vPos);
//	m_vMovePos = vPos;				// 마지막에 나옴 vPos를 멤버변수에 넣어서 저장해준다.
}

void CPlayer::KeyMouse_Input(const _float & fTimeDelta)
{
	if (m_pKeyMgr->KeyPressing(KEY_LBUTTON))
	{
		m_fAttTimer = 2.f;
		m_eCurState = CGameObject::ATTACK;
		Basic_AttackCombo(fTimeDelta, &m_dEndTime);
	}
	else
	{
		m_iAttStack = 0;
		m_fAttTimer = 2.f;
		if (m_pMeshCom->Is_AnimationSetEnd(m_dEndTime))
		{
			Attack_Landing();					// 공격마다 착지동작이 달라질 수 있음.
		}
	}
}

void CPlayer::Public_Key_Input()
{
	if (m_pKeyMgr->KeyDown(KEY_SHIFT))
	{
		Change_PlayerPose();				//탭을 누를 때마다 변경해줌.
		if (CGameObject::STAND == m_eCurState)
			m_pMeshCom->Set_AnimationSet(18 + m_ePose * 9 / 4);
	}
}

void CPlayer::Motion_Event(const _float& fTimeDelta)
{
	_vec3 vPos = {};
	switch (m_eCurState)
	{
	case Engine::CGameObject::STAND:
	case Engine::CGameObject::WALK:
		m_iAttStack = 0;
		m_fAttTimer = 2.f;				// 평소 상태에선 공격할때의 시간값을 맞춰준다.
		WalkDust(fTimeDelta);
		m_eHitOption = Engine::CGameObject::HIT_NORMAL;
		m_bIsAttEffect = false;
		break;
	case Engine::CGameObject::ATTACK:
//		Attack_Landing();		// 공격시 착륙 함수를 호출.
		break;
	case Engine::CGameObject::DAMAGE:
		if (m_pMeshCom->Is_AnimationSetEnd())
		{
			m_eCurState = CGameObject::STAND;
			m_eHitOption = CGameObject::HIT_NORMAL;
			//m_bIsHit = false;
		}
		if (CGameObject::HIT_KNOCKBACK == m_eHitOption)
		{
			WalkDust(fTimeDelta);
			_vec3 vLook = m_pTransCom->m_vInfo[Engine::INFO_LOOK];
			D3DXVec3Normalize(&vLook, &vLook);

			vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vLook * fTimeDelta * m_fSpeed * -0.8f));
			m_pTransCom->Set_Position(&vPos);
		}
		break;
	case Engine::CGameObject::JUMP:
		if (m_pMeshCom->Is_AnimationSetEnd())
		{
			m_eCurState = CGameObject::STAND;
			if(m_pMeshCom->Is_AnimationSetEnd())
				Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"Dust", CDust::Create(m_pGraphicDev, *m_pTransCom->Get_Info(Engine::INFO_POS), CDust::ROLLING));
		}

		vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(m_vMoveDir * fTimeDelta * m_fSpeed));
		m_pTransCom->Set_Position(&vPos);
		break;
	case Engine::CGameObject::DIE:
		break;
	default:
		break;
	}
}


HRESULT CPlayer::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix		matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &m_pTransCom->m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	Safe_Release(pEffect);

	return S_OK;
}

void CPlayer::MotionChange()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case CGameObject::STAND:
			m_pMeshCom->Set_AnimationSet(18 + m_ePose * 9 / 4);
			m_pBoneName = "Bip01_Spine";
			m_bIsJumpOK = false;
			m_eAttMode = BASIC;
			break;
		case CGameObject::ATTACK:
		//	m_pMeshCom->Set_AnimationSet(15);
			
			m_pBoneName = "_Main";
			break;
		case CGameObject::DAMAGE:
			if (CGameObject::ATTACK == m_ePreState) // 바로 전 동작이 공격동작이라면
				m_eCurState = CGameObject::ATTACK;
			else
			{
				m_pMeshCom->Set_AnimationSet(2);		// Hit
				m_pBoneName = "Bip01_Spine";
			}
			{
				_matrix matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);
				_vec3 vHitPos = {};
				memcpy(&vHitPos, matBone.m[3], sizeof(_vec3));

				Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"HitEffect", CHitEffect::Create(m_pGraphicDev, vHitPos, L"Texture_Player_Hit"));
			}
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);
			CSoundMgr::GetInstance()->MyPlaySound(L"player_machina_bvh_01_b.ogg", CSoundMgr::PLAYER);

			break;
		case CGameObject::JUMP:
			break;
		default:
			break;
		}
		m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);

		m_ePreState = m_eCurState;
	}
}

void CPlayer::Attack_Landing()
{
	if (m_pMeshCom->Is_AnimationSetEnd(m_dEndTime))
	{
		switch (m_eAttMode)
		{
		case CPlayer::BASIC:
			m_bIsJumpOK = false;
		case CPlayer::LEAPINGOVER:
			m_pMeshCom->Set_AnimationSet(0);
			m_eCurState = CGameObject::STAND;
			m_dEndTime = 0.1;
			break;
		case CPlayer::KIDNEYBLOW:
			m_pMeshCom->Set_AnimationSet(9);
			m_eAttMode = BASIC;
			m_dEndTime = 0.1;
			break;

		}
	}
}

void CPlayer::Basic_AttackCombo(const _float & fTimeDelta, _double* pEndTime)
{
	// 콤보공격 순서 : 17 -> 15 -> 13 -> 12 -> 11
	// 착지 : 17->16, 15->14, 나머지 : stand 상태.
	// Bip01_R_Toe0 : 발. _Main : 주먹
	_vec3 vPos = {};
	switch (m_iAttStack)
	{
	case 0:
		m_pMeshCom->Set_AnimationSet(17); // 첫 공격.
		*pEndTime = 2.0;
		m_eHitOption = Engine::CGameObject::HIT_NORMAL;

		vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(m_vMoveDir * fTimeDelta * m_fSpeed));
		m_pTransCom->Set_Position(&vPos);
		m_pBoneName = "_Main";

		Engine::Get_Layer(L"GameLogic")->CollisionObject(L"TinyDragon", m_matBone, m_fRadius, m_eHitOption);
		CSoundMgr::GetInstance()->MyPlaySound(L"machina_att_1.ogg", CSoundMgr::PLAYER_ATTACK1);
		break;
	case 1:
		m_pMeshCom->Set_AnimationSet(15);
		*pEndTime = 1.0;
		m_eHitOption = Engine::CGameObject::HIT_NORMAL;

		vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(m_vMoveDir * fTimeDelta * m_fSpeed));
		m_pTransCom->Set_Position(&vPos);
		if (!m_bIsJumpOK)
		{
			m_bIsJump = true;
			m_bIsJumpOK = true;
			m_fJumpPower = 2.f;
		}
		m_pBoneName = "Bip01_R_Toe0";

		Engine::Get_Layer(L"GameLogic")->CollisionObject(L"TinyDragon", m_matBone, m_fRadius, m_eHitOption);
		CSoundMgr::GetInstance()->MyPlaySound(L"machina_att_2.ogg", CSoundMgr::PLAYER_ATTACK2);

		break;
	case 2:
		m_pMeshCom->Set_AnimationSet(13);
		*pEndTime = 2.5;
		m_pBoneName = "_Main";
		m_eHitOption = Engine::CGameObject::HIT_NORMAL;

		Engine::Get_Layer(L"GameLogic")->CollisionObject(L"TinyDragon", m_matBone, m_fRadius, m_eHitOption);
		CSoundMgr::GetInstance()->MyPlaySound(L"machina_att_3.ogg", CSoundMgr::PLAYER_ATTACK3);

		break;
	case 3:
		m_pMeshCom->Set_AnimationSet(12);
		*pEndTime = 4.5;
		m_eHitOption = Engine::CGameObject::HIT_NORMAL;

		m_pBoneName = "Bip01_R_Toe0";
		Engine::Get_Layer(L"GameLogic")->CollisionObject(L"TinyDragon", m_matBone, m_fRadius, m_eHitOption);
		CSoundMgr::GetInstance()->MyPlaySound(L"machina_att_4.ogg", CSoundMgr::PLAYER_ATTACK4);

		break;
	case 4:
		m_pMeshCom->Set_AnimationSet(11);
		*pEndTime = 1.0;
		m_pBoneName = "_Main";
		if (m_pMeshCom->Is_AnimationAfterTrack(1.f))
		{
			m_eHitOption = Engine::CGameObject::HIT_KNOCKBACK;
			Engine::Get_Layer(L"GameLogic")->CollisionObject(L"TinyDragon", m_matBone, m_fRadius, m_eHitOption);

			_vec3 vLook = m_pTransCom->m_vInfo[Engine::INFO_LOOK];
			D3DXVec3Normalize(&vLook, &vLook);

			vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vLook * fTimeDelta * m_fSpeed * 0.5f));
			m_pTransCom->Set_Position(&vPos);

			if (!m_bIsAttEffect)
			{
				Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"Dust", CDust::Create(m_pGraphicDev, *m_pTransCom->Get_Info(Engine::INFO_POS), CDust::JUMP));
				m_bIsAttEffect = true;
			}
		}
		CSoundMgr::GetInstance()->MyPlaySound(L"machina_att_5.ogg", CSoundMgr::PLAYER_ATTACK5);

		break;
	default:
		break;
	}

	if (m_iAttStack < 5 && m_pMeshCom->Is_AnimationSetEnd(*pEndTime))			// KeyPressing시 마지막 동작을 제외하고 나머지는.
	{
		Engine::Get_Layer(L"GameLogic")->Reset_HitObject(L"TinyDragon");
		m_iAttStack++;
	}

	if (m_iAttStack >= 5 && m_pMeshCom->Is_AnimationSetEnd(*pEndTime))			// 마지막 동작에 대한 처리.
	{
		m_iAttStack = 0;
	}

	m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);			// 부딪힐 것 처리.
}

void CPlayer::IsJumping(const _float& fTimeDelta)
{
	_float fPosY = 0.f;
	
	if (m_bIsJump)
	{
		m_fJumpAccel += fTimeDelta;

		fPosY += m_fJumpPower * m_fJumpAccel - 9.8f * m_fJumpAccel * m_fJumpAccel * 0.5f;

		if (0.01f > fPosY)
		{
			fPosY = m_pTransCom->m_vInfo[Engine::INFO_POS].y; // 각 네비 높이로 태워야함.
			m_bIsJump = false;
			m_fJumpAccel = 0.f;
			Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"Dust", CDust::Create(m_pGraphicDev, *m_pTransCom->Get_Info(Engine::INFO_POS), CDust::JUMP));

		}
	}
	else
	{
		fPosY = m_pTransCom->m_vInfo[Engine::INFO_POS].y; // 점프 안할땐 y값 그대로 따라간다.

		m_fJumpPower = 0.f;
		if (0.01f > fPosY)
		{
			fPosY = 0.01f; // 각 네비 높이로 태워야함.
			m_fJumpAccel = 0.f;
		}
	}

	m_pTransCom->m_vInfo[Engine::INFO_POS].y = fPosY;
}

void CPlayer::KidneyBlow(const _float & fTimeDelta)
{
	if (KIDNEYBLOW != m_eAttMode)			//현재 공격모드가 해당 스킬 사용이 아니라면.
		return;
	m_dEndTime = 0.1;	// 끝나는 시간은 일반 공격과 마찬가지로.
	if (m_pMeshCom->Is_AnimationSetEnd(4.5f))
	{
		_vec3 vLook = m_pTransCom->m_vInfo[Engine::INFO_LOOK];
		D3DXVec3Normalize(&vLook, &vLook);

		_vec3 vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vLook * fTimeDelta * m_fSpeed * 1.5f));
		m_pTransCom->Set_Position(&vPos);
	}
}

void CPlayer::LeapingOver(const _float & fTimeDelta)
{
	if (LEAPINGOVER != m_eAttMode)			//현재 공격모드가 해당 스킬 사용이 아니라면.
		return;

	if (m_pMeshCom->Is_AnimationSetEnd(2.f))
	{
		m_dEndTime = 0.3;	// 끝나는 시간은 일반 공격과 마찬가지로.

		if (m_bIsJumpOK && !m_bIsJump)
		{
			m_bIsJumpOK = false;
			m_bIsJump = false;
		}
		else
		{
			m_fJumpPower = 5.f;
			m_bIsJumpOK = true;
			m_bIsJump = true;
		}
	}
	else if (m_pMeshCom->Is_AnimationSetEnd(4.5f))
	{
		m_dEndTime = 0.1;	// 끝나는 시간은 일반 공격과 마찬가지로.

		if (!m_bIsJumpOK)
		{
			m_fJumpPower = 3.f;
			m_bIsJumpOK = true;
			m_bIsJump = true;
		}
	}
}

void CPlayer::WalkDust(const _float & fTimeDelta)
{
	// 0.1초 마다 생성해야하는것같음.??
	if (CGameObject::WALK == m_eCurState)
	{
		m_fDustTimer -= fTimeDelta;	// 매 프레임 별로 빼준다.

		if (0.f >= m_fDustTimer)
		{
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);
			CSoundMgr::GetInstance()->MyPlaySound(L"female_run_stone_c.wav", CSoundMgr::PLAYER);
			m_fDustTimer = 0.25f;
			Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"Dust", CDust::Create(m_pGraphicDev, *m_pTransCom->Get_Info(Engine::INFO_POS), CDust::WALK));
		}
	}
	else if(CGameObject::DAMAGE == m_eCurState)
	{
		if (m_eHitOption == CGameObject::HIT_KNOCKBACK)
		{
			m_fDustTimer -= fTimeDelta;	// 매 프레임 별로 빼준다.

			if (0.f >= m_fDustTimer)
			{
				m_fDustTimer = 0.1f;
				Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"Dust", CDust::Create(m_pGraphicDev, *m_pTransCom->Get_Info(Engine::INFO_POS), CDust::WALK));
			}
		}
	}
	else
		m_fDustTimer = 0.25f;
}

void CPlayer::Create_Cell(const _vec3 & vPos, const Engine::CCell::CELL_OPTION & eOption)
{
	m_pNaviMeshCom->Set_CellPos(vPos, eOption);
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh)
{
	CPlayer* pInstance = new CPlayer(pGraphicDev);


	if (FAILED(pInstance->Ready_Object(0)))
	{
		ERR_BOX("Player Created Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->Set_NaviCom(pNaviMesh);					// Terrain의 NaviMesh를 세팅한다

	return pInstance;
}

void CPlayer::Free(void)
{
	Engine::CGameObject::Free();
}