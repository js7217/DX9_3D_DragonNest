#include "stdafx.h"
#include "Goblin.h"

#include "Export_Function.h"
#include "Player.h"
#include "Sword.h"
#include "HitEffect.h"

CGoblin::CGoblin(LPDIRECT3DDEVICE9 pGraphicDev, CPlayer* pPlayer)
	: Engine::CGameObject(pGraphicDev)
	, m_vDir(0.f, 0.f, 0.f)
	, m_bIsColl(false), m_bIsDead(false), m_wstrName(L"Goblin")
	, m_pTarget(pPlayer)
{

}

CGoblin::~CGoblin(void)
{

}

Engine::CTransform * CGoblin::Get_TransformCom()
{
	return m_pTransCom;
}

void CGoblin::Set_Pos(const _vec3 & vPos)
{
	m_pTransCom->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CGoblin::Set_Size(const _vec3 & vSize)
{
	m_pTransCom->Set_Scale(vSize.x, vSize.y, vSize.z);
	m_pBoneColliderCom->Set_Radius(m_pTransCom->m_vScale);		// 객체 크기에 따라 구의 크기가 바뀐다.
	m_fRadius = m_pBoneColliderCom->Get_Radius();				// 만들어진 반지름을 가져온다.
}

void CGoblin::Set_Rotate(const _vec3 & vRotate)
{
	m_pTransCom->m_vAngle = vRotate;
}

void CGoblin::Set_NaviCom(Engine::CNaviMesh * pNaviMesh)
{
	_int iIndex = 0;
	m_pNaviMeshCom->Get_PickCell(m_pTransCom->m_vInfo[Engine::INFO_POS], &iIndex);
	m_pNaviMeshCom->SetCurrentIndex(iIndex);
}

HRESULT CGoblin::Ready_Object(const _int& iFlag)
{
	Engine::CGameObject::Ready_Object();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fSpeed = 0.3f;
//	m_iHp = 10;
	m_fAniSpeed = 2.f;
	m_pMeshCom->Set_AnimationSet(1); // Stand

	m_iFlag = iFlag;
	// Bip01-Head
	m_pBoneColliderCom->Set_BoneName("Bip01_R_Hand", m_pMeshCom, &(m_pTransCom->m_matWorld));
	m_pBoneColliderCom->Set_BoneName("Bip01_Spine1", m_pMeshCom, &(m_pTransCom->m_matWorld));			// 가져오고 싶은 행렬의 뼈 이름
	m_pBoneName = "Bip01_Spine1";

	m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);	// 뼈

	return S_OK;
}

_int CGoblin::Update_Object(const _float& fTimeDelta)
{
	if (!m_bIsOpen)
		return 0;
	if (m_bIsDead)
	{
		if (m_pMeshCom->Is_AnimationSetEnd())
			return 1;
	}

	Engine::CGameObject::Update_Object(fTimeDelta);
//	Set_OnTerrain();
	m_fTimeDelta = fTimeDelta;

	MotionChange();
	Chase_Target();	// 타겟 추적..
	MotionEvent();

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	m_pSword->Update_Object(fTimeDelta);
	return 0;
}

void CGoblin::Render_Object(void)
{
	if (!m_bIsOpen)
		return;
	m_pMeshCom->Play_AnimationSet(m_fTimeDelta * m_fAniSpeed); // 순서 유의..?
	m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);	// 뼈


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


	m_pSword->Render_Object();
	// Collider
//	m_pBoneColliderCom->Render_Collider();

}

void CGoblin::Set_HitEffect()
{
	_matrix matBone = m_pBoneColliderCom->Current_BoneMatrix("Bip01_Spine1");
	_vec3 vHitPos = {};
	memcpy(&vHitPos, matBone.m[3], sizeof(_vec3));

	Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"HitEffect", CHitEffect::Create(m_pGraphicDev, vHitPos, L"Texture_Hit"));
}

HRESULT CGoblin::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pTransCom = Engine::CTransform::Create(_vec3(0.f, 0.f, 1.f));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);
	
	pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_CalculatorCom", pComponent);

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_Goblin"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	pComponent = m_pBoneColliderCom = Engine::CBoneCollider::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_BoneCollider", pComponent);

	// NaviMesh
	pComponent = m_pNaviMeshCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Clone(L"Proto_NaviMesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);

	return S_OK;
}

HRESULT CGoblin::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix		matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &m_pTransCom->m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//const D3DLIGHT9*	pLightInfo = Engine::Get_LightInfo();
	//NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	//pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));

	//pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLightInfo->Diffuse);
	//pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLightInfo->Ambient);
	//pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLightInfo->Specular);

	//D3DMATERIAL9		tMtrlInfo;
	//ZeroMemory(&tMtrlInfo, sizeof(D3DMATERIAL9));

	//tMtrlInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	//tMtrlInfo.Power = 20.f;

	//pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrlInfo.Diffuse);
	//pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrlInfo.Ambient);
	//pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrlInfo.Specular);
	//pEffect->SetFloat("g_fPower", tMtrlInfo.Power);

	//D3DXMatrixInverse(&matView, NULL, &matView);
	//pEffect->SetVector("g_vCamPos", (_vec4*)&matView._41);


	Safe_Release(pEffect);

	return S_OK;
}

void CGoblin::Chase_Target()				// 타겟을 따라가는 함수.
{
	if (m_eCurState == CGameObject::DIE || m_eCurState == CGameObject::DAMAGE || m_eCurState == CGameObject::ATTACK)
		return;
	_vec3 vTargetPos = m_pTarget->Get_TransformCom()->m_vInfo[Engine::INFO_POS];

	_float fLength = D3DXVec3Length(&(vTargetPos - m_pTransCom->m_vInfo[Engine::INFO_POS]));
	if (0.2f >= fLength)							// 거리가 0.2이하 일때 : 공격모션 취하기.
	{
		m_eCurState = CGameObject::ATTACK;

		m_pTransCom->Chase_Target_RotY(vTargetPos, Engine::ROT_Z);			// 타겟을 향해 방향을 틀어준다.
	}
	else if (1.f >= fLength || m_bIsFirstHit)						// 거리가 1.f이하 일때. : 쫓아오기.
	{
		m_eCurState = CGameObject::WALK;

		m_pTransCom->Chase_Target_RotY(vTargetPos, Engine::ROT_Z);			// 타겟을 향해 방향을 틀어준다.
		_vec3 vDir = m_pTransCom->Get_Dir(Engine::ROT_Z);		// 현재 matWorld가 업데이트 된 상황에서의 Look(메쉬가 바라보고있는방향마다 다름)
		_vec3 vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * m_fTimeDelta * m_fSpeed));
		m_pTransCom->Set_Position(&vPos);
	}
	else
		m_eCurState = CGameObject::STAND;
}

void CGoblin::MotionChange()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Engine::CGameObject::STAND:
			m_pMeshCom->Set_AnimationSet(1);
			m_pBoneName = "Bip01_Spine1";
			m_fAniSpeed = 2.f;
			break;
		case Engine::CGameObject::WALK:
			m_pMeshCom->Set_AnimationSet(2);
			m_pBoneName = "Bip01_Spine1";
			m_fAniSpeed = 2.f;

			break;
		case Engine::CGameObject::ATTACK:
			m_pMeshCom->Set_AnimationSet(6);
			m_pBoneName = "Bip01_R_Hand";
			// 입쪽으로 바꿔야함.
			Engine::Get_Layer(L"GameLogic")->Reset_HitObject(L"Player");
			m_fAniSpeed = 2.f;
			break;
		case Engine::CGameObject::DAMAGE:
			switch (m_eHitOption)
			{
			case CGameObject::HIT_NORMAL:
				m_pMeshCom->Set_AnimationSet(7);
				m_fAniSpeed = 5.f;
				break;
			case CGameObject::HIT_KNOCKBACK:
				m_pMeshCom->Set_AnimationSet(0);
				m_fAniSpeed = 2.f;
				break;
			}
			m_pBoneName = "Bip01_Spine1";
			m_bIsFirstHit = true;
			CSoundMgr::GetInstance()->MyPlaySound(L"goblin_cry_short_a.wav", CSoundMgr::GOBLIN_HIT);

			break;
		case Engine::CGameObject::DIE:
			m_pMeshCom->Set_AnimationSet(8);
			m_pBoneName = "Bip01_Spine1";
			m_fAniSpeed = 2.f;

			m_bIsDead = true;
			CSoundMgr::GetInstance()->MyPlaySound(L"goblin_dead_a.wav", CSoundMgr::GOBLIN_DEAD);

			break;
		default:
			break;
		}
		m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);

		m_ePreState = m_eCurState;
	}
}

void CGoblin::MotionEvent()
{
	switch (m_eCurState)
	{
	case Engine::CGameObject::STAND:
		break;
	case Engine::CGameObject::WALK:
		break;
	case Engine::CGameObject::ATTACK:
		if (m_ePreState != CGameObject::ATTACK)
			break;
		if (m_pMeshCom->Is_AnimationAfterTrack(0.8))
		{
			Engine::Get_Layer(L"GameLogic")->CollisionObject(L"Player", m_matBone, m_fRadius);
			CSoundMgr::GetInstance()->MyPlaySound(L"goblin_attack_a.wav", CSoundMgr::GOBLIN_ATTACK);
		}
		if (m_pMeshCom->Is_AnimationSetEnd())
			m_eCurState = CGameObject::STAND;
			//Engine::Get_Layer(L"GameLogic")->Reset_HitObject(L"Player");			//플레이어가 다시 맞게끔해야한다.
		break;
	case Engine::CGameObject::DAMAGE:
		if (m_pMeshCom->Is_AnimationSetEnd())
			m_eCurState = CGameObject::STAND;

		if (m_iHp <= 0)
			m_eCurState = CGameObject::DIE;
		else
		{
			switch (m_eHitOption)
			{
			case Engine::CGameObject::HIT_NORMAL:
				{
					_vec3 vDir = m_pTransCom->Get_Dir(Engine::ROT_Z);		// 현재 matWorld가 업데이트 된 상황에서의 Look(메쉬가 바라보고있는방향마다 다름)
					_vec3 vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * m_fTimeDelta * -m_fSpeed * 0.8f));
					m_pTransCom->Set_Position(&vPos);
				}
				break;
			case Engine::CGameObject::HIT_KNOCKBACK:
				if (!m_pMeshCom->Is_AnimationSetEnd(0.5))	//날아가는 것.
				{
					_vec3 vDir = m_pTransCom->Get_Dir(Engine::ROT_Z);		// 현재 matWorld가 업데이트 된 상황에서의 Look(메쉬가 바라보고있는방향마다 다름)
					_vec3 vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * m_fTimeDelta * -m_fSpeed * 2.f));
					m_pTransCom->Set_Position(&vPos);
				}
				break;
			default:
				break;
			}
		}
		break;
	case Engine::CGameObject::JUMP:
		break;
	case Engine::CGameObject::DIE:
		break;
	default:
		break;
	}
}

CGoblin* CGoblin::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh, CPlayer* pPlayer)
{
	CGoblin* pInstance = new CGoblin(pGraphicDev, pPlayer);


	if (FAILED(pInstance->Ready_Object(0)))
	{
		ERR_BOX("Goblin Created Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->Set_NaviCom(pNaviMesh);					// Terrain의 NaviMesh를 세팅한다

	return pInstance;
}

CGoblin * CGoblin::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh * pNaviMesh, CPlayer* pPlayer,
	const _vec3 & vPos, const _vec3 & vSize, const _vec3 & vRotate, const _uint& iGroup)
{
	CGoblin* pInstance = new CGoblin(pGraphicDev, pPlayer);

	if (FAILED(pInstance->Ready_Object(0)))
	{
		ERR_BOX("Goblin Created Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->Set_Pos(vPos);
	pInstance->Set_Size(vSize);
	pInstance->Set_Rotate(vRotate);
	pInstance->Set_NaviCom(pNaviMesh);					// Terrain의 NaviMesh를 세팅한다
	pInstance->Set_Group(iGroup);
	return pInstance;
}

void CGoblin::Free(void)
{
	if(nullptr != m_pSword)
		Engine::Safe_Release(m_pSword);
	Engine::CGameObject::Free();
}