#include "stdafx.h"
#include "Spartan.h"

#include "Export_Function.h"
#include "Player.h"
#include "Shovel.h"
#include "HitEffect.h"

CSpartan::CSpartan(LPDIRECT3DDEVICE9 pGraphicDev, CPlayer* pPlayer)
	: Engine::CGameObject(pGraphicDev)
	, m_vDir(0.f, 0.f, 0.f)
	, m_bIsColl(false), m_bIsDead(false), m_wstrName(L"Spartan")
	, m_pTarget(pPlayer)
{

}

CSpartan::~CSpartan(void)
{

}

Engine::CTransform * CSpartan::Get_TransformCom()
{
	return m_pTransCom;
}

void CSpartan::Set_Pos(const _vec3 & vPos)
{
	m_pTransCom->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CSpartan::Set_Size(const _vec3 & vSize)
{
	m_pTransCom->Set_Scale(vSize.x, vSize.y, vSize.z);
	m_pBoneColliderCom->Set_Radius(m_pTransCom->m_vScale);		// ��ü ũ�⿡ ���� ���� ũ�Ⱑ �ٲ��.
	m_fRadius = m_pBoneColliderCom->Get_Radius();				// ������� �������� �����´�.
}

void CSpartan::Set_Rotate(const _vec3 & vRotate)
{
	m_pTransCom->m_vAngle = vRotate;
}

void CSpartan::Set_NaviCom(Engine::CNaviMesh * pNaviMesh)
{
	_int iIndex = 0;
	m_pNaviMeshCom->Get_PickCell(m_pTransCom->m_vInfo[Engine::INFO_POS], &iIndex);
	m_pNaviMeshCom->SetCurrentIndex(iIndex);
}

HRESULT CSpartan::Ready_Object(const _int& iFlag)
{
	Engine::CGameObject::Ready_Object();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fSpeed = 0.2f;
//	m_iHp = 10;
	m_fAniSpeed = 2.f;
	m_pMeshCom->Set_AnimationSet(5); // Stand

	m_iFlag = iFlag;
	// Bip01-Head
	m_pBoneColliderCom->Set_BoneName("Bip01_R_Hand", m_pMeshCom, &(m_pTransCom->m_matWorld));
	m_pBoneColliderCom->Set_BoneName("Bip01_Spine1", m_pMeshCom, &(m_pTransCom->m_matWorld));			// �������� ���� ����� �� �̸�
	m_pBoneName = "Bip01_Spine1";

	m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);	// ��

	return S_OK;
}

_int CSpartan::Update_Object(const _float& fTimeDelta)
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
	Chase_Target();	// Ÿ�� ����..
	MotionEvent();

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	m_pSword->Update_Object(fTimeDelta);
	return 0;
}

void CSpartan::Render_Object(void)
{
	if (!m_bIsOpen)
		return;
	m_pMeshCom->Play_AnimationSet(m_fTimeDelta * m_fAniSpeed); // ���� ����..?
	m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);	// ��


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

void CSpartan::Set_HitEffect()
{
	_matrix matBone = m_pBoneColliderCom->Current_BoneMatrix("Bip01_Spine1");
	_vec3 vHitPos = {};
	memcpy(&vHitPos, matBone.m[3], sizeof(_vec3));

	Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"HitEffect", CHitEffect::Create(m_pGraphicDev, vHitPos, L"Texture_Hit"));
}

HRESULT CSpartan::Add_Component(void)
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
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_Spartan"));
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

HRESULT CSpartan::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	const Engine::D3DXFRAME_DERIVED* pRootBone = m_pMeshCom->Get_FrameByName("Bip01");
	_matrix matRootBone = pRootBone->CombinedTransformMatrix;

	_matrix		matView, matProj, matBone;
	D3DXMatrixIdentity(&matBone);

	memcpy(matBone.m[3], matRootBone.m[3], sizeof(_vec3));
	matBone._42 = 0.f;
	D3DXMatrixInverse(&matBone, nullptr, &matBone);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &(matBone * m_pTransCom->m_matWorld));
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);


	Safe_Release(pEffect);

	return S_OK;
}

void CSpartan::Chase_Target()				// Ÿ���� ���󰡴� �Լ�.
{
	if (m_eCurState == CGameObject::DIE || m_eCurState == CGameObject::DAMAGE || m_eCurState == CGameObject::ATTACK)
		return;
	_vec3 vTargetPos = m_pTarget->Get_TransformCom()->m_vInfo[Engine::INFO_POS];

	_float fLength = D3DXVec3Length(&(vTargetPos - m_pTransCom->m_vInfo[Engine::INFO_POS]));
	if (2.f >= fLength && !m_pSword->Get_Throw())							// �Ÿ��� 0.2���� �϶� : ���ݸ�� ���ϱ�.
	{
		m_eCurState = CGameObject::ATTACK;

		m_pTransCom->Chase_Target_RotY(vTargetPos, Engine::ROT_Z);			// Ÿ���� ���� ������ Ʋ���ش�.
	}
	else if (2.5f >= fLength || m_bIsFirstHit)						// �Ÿ��� 1.f���� �϶�. : �Ѿƿ���.
	{
		m_eCurState = CGameObject::WALK;

		m_pTransCom->Chase_Target_RotY(vTargetPos, Engine::ROT_Z);			// Ÿ���� ���� ������ Ʋ���ش�.
		_vec3 vDir = m_pTransCom->Get_Dir(Engine::ROT_Z);		// ���� matWorld�� ������Ʈ �� ��Ȳ������ Look(�޽��� �ٶ󺸰��ִ¹��⸶�� �ٸ�)
		_vec3 vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * m_fTimeDelta * m_fSpeed * 0.6f));
		m_pTransCom->Set_Position(&vPos);
	}
	else
		m_eCurState = CGameObject::STAND;
}

void CSpartan::MotionChange()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Engine::CGameObject::STAND:
			m_pMeshCom->Set_AnimationSet(5);
			m_pBoneName = "Bip01_Spine1";
			m_fAniSpeed = 2.f;
			break;
		case Engine::CGameObject::WALK:
			m_pMeshCom->Set_AnimationSet(4);
			m_pBoneName = "Bip01_Spine1";
			m_fAniSpeed = 2.f;

			break;
		case Engine::CGameObject::ATTACK:
			m_pMeshCom->Set_AnimationSet(3);
			m_pBoneName = "Bip01_R_Hand";
			// �������� �ٲ����.
			Engine::Get_Layer(L"GameLogic")->Reset_HitObject(L"Player");
			m_fAniSpeed = 2.f;
			break;
		case Engine::CGameObject::DAMAGE:
			switch (m_eHitOption)
			{
			case CGameObject::HIT_NORMAL:
				m_pMeshCom->Set_AnimationSet(2);
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
			m_pMeshCom->Set_AnimationSet(1);
			m_pBoneName = "Bip01_Spine1";
			m_fAniSpeed = 2.f;
			CSoundMgr::GetInstance()->MyPlaySound(L"goblin_dead_a.wav", CSoundMgr::GOBLIN_DEAD);

			m_bIsDead = true;
			break;
		default:
			break;
		}
		m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);

		m_ePreState = m_eCurState;
	}
}

void CSpartan::MotionEvent()
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
		if (m_pMeshCom->Is_AnimationAfterTrack(1.6))
		{
			CSoundMgr::GetInstance()->MyPlaySound(L"goblin_attack_a.wav", CSoundMgr::GOBLIN_ATTACK);
			m_pSword->Set_Throw(m_pTransCom->m_vAngle.y);
		}
			//Engine::Get_Layer(L"GameLogic")->CollisionObject(L"Player", m_matBone, m_fRadius);
		if (m_pMeshCom->Is_AnimationSetEnd())
			m_eCurState = CGameObject::STAND;
			//Engine::Get_Layer(L"GameLogic")->Reset_HitObject(L"Player");			//�÷��̾ �ٽ� �°Բ��ؾ��Ѵ�.
		if(m_pSword->Get_Throw())
			m_eCurState = CGameObject::WALK;
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
					_vec3 vDir = m_pTransCom->Get_Dir(Engine::ROT_Z);		// ���� matWorld�� ������Ʈ �� ��Ȳ������ Look(�޽��� �ٶ󺸰��ִ¹��⸶�� �ٸ�)
					_vec3 vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * m_fTimeDelta * -m_fSpeed * 0.8f));
					m_pTransCom->Set_Position(&vPos);
				}
				break;
			case Engine::CGameObject::HIT_KNOCKBACK:
				if (!m_pMeshCom->Is_AnimationSetEnd(0.5))	//���ư��� ��.
				{
					_vec3 vDir = m_pTransCom->Get_Dir(Engine::ROT_Z);		// ���� matWorld�� ������Ʈ �� ��Ȳ������ Look(�޽��� �ٶ󺸰��ִ¹��⸶�� �ٸ�)
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

CSpartan* CSpartan::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh, CPlayer* pPlayer)
{
	CSpartan* pInstance = new CSpartan(pGraphicDev, pPlayer);


	if (FAILED(pInstance->Ready_Object(0)))
	{
		ERR_BOX("Spartan Created Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->Set_NaviCom(pNaviMesh);					// Terrain�� NaviMesh�� �����Ѵ�

	return pInstance;
}

CSpartan * CSpartan::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh * pNaviMesh, CPlayer* pPlayer,
	const _vec3 & vPos, const _vec3 & vSize, const _vec3 & vRotate, const _uint& iGroup)
{
	CSpartan* pInstance = new CSpartan(pGraphicDev, pPlayer);

	if (FAILED(pInstance->Ready_Object(0)))
	{
		ERR_BOX("Spartan Created Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->Set_Pos(vPos);
	pInstance->Set_Size(vSize);
	pInstance->Set_Rotate(vRotate);
	pInstance->Set_NaviCom(pNaviMesh);					// Terrain�� NaviMesh�� �����Ѵ�
	pInstance->Set_Group(iGroup);
	return pInstance;
}

void CSpartan::Free(void)
{
	if(nullptr != m_pSword)
		Engine::Safe_Release(m_pSword);
	Engine::CGameObject::Free();
}