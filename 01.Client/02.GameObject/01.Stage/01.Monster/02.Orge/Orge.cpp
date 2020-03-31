#include "stdafx.h"
#include "Orge.h"

#include "Export_Function.h"
#include "Player.h"
#include "Sword.h"
#include "HitEffect.h"

COrge::COrge(LPDIRECT3DDEVICE9 pGraphicDev, CPlayer* pPlayer)
	: Engine::CGameObject(pGraphicDev)
	, m_vDir(0.f, 0.f, 0.f)
	, m_bIsColl(false), m_bIsDead(false), m_wstrName(L"Orge")
	, m_pTarget(pPlayer)
{

}

COrge::~COrge(void)
{

}

Engine::CTransform * COrge::Get_TransformCom()
{
	return m_pTransCom;
}

void COrge::Set_Pos(const _vec3 & vPos)
{
	m_pTransCom->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void COrge::Set_Size(const _vec3 & vSize)
{
	m_pTransCom->Set_Scale(vSize.x, vSize.y, vSize.z);
	m_pBoneColliderCom->Set_Radius(m_pTransCom->m_vScale);		// 객체 크기에 따라 구의 크기가 바뀐다.
	m_fRadius = m_pBoneColliderCom->Get_Radius();				// 만들어진 반지름을 가져온다.
}

void COrge::Set_Rotate(const _vec3 & vRotate)
{
	m_pTransCom->m_vAngle = vRotate;
}

void COrge::Set_NaviCom(Engine::CNaviMesh * pNaviMesh)
{
	_int iIndex = 0;
	m_pNaviMeshCom->Get_PickCell(m_pTransCom->m_vInfo[Engine::INFO_POS], &iIndex);
	m_pNaviMeshCom->SetCurrentIndex(iIndex);
}

HRESULT COrge::Ready_Object(const _int& iFlag)
{
	Engine::CGameObject::Ready_Object();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fSpeed = 0.3f;
//	m_iHp = 10;
	m_fAniSpeed = 2.f;
	m_pMeshCom->Set_AnimationSet(2); // Stand

	m_iFlag = iFlag;

	// Bip01-Head
	m_pBoneColliderCom->Set_BoneName("Bip01_R_Hand", m_pMeshCom, &(m_pTransCom->m_matWorld));
	m_pBoneColliderCom->Set_BoneName("Bip01_Spine", m_pMeshCom, &(m_pTransCom->m_matWorld));			// 가져오고 싶은 행렬의 뼈 이름
	m_pBoneName = "Bip01_Spine";

	m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);	// 뼈

	const Engine::D3DXFRAME_DERIVED* pRootBone = m_pMeshCom->Get_FrameByName("Bip01");
	m_matRootBone = pRootBone->CombinedTransformMatrix;

	return S_OK;
}

_int COrge::Update_Object(const _float& fTimeDelta)
{
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

void COrge::Render_Object(void)
{
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

void COrge::Set_HitEffect()
{
	_matrix matBone = m_pBoneColliderCom->Current_BoneMatrix("Bip01_Spine");
	_vec3 vHitPos = {};
	memcpy(&vHitPos, matBone.m[3], sizeof(_vec3));

	Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"HitEffect", CHitEffect::Create(m_pGraphicDev, vHitPos, L"Texture_Hit"));
}

HRESULT COrge::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pTransCom = Engine::CTransform::Create(_vec3(0.f, 0.f, -1.f));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);
	
	pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_CalculatorCom", pComponent);

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_Orge"));
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

HRESULT COrge::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	const Engine::D3DXFRAME_DERIVED* pRootBone = m_pMeshCom->Get_FrameByName("Bip01");
	m_matRootBone = pRootBone->CombinedTransformMatrix;

	_matrix		matView, matProj, matBone;
	D3DXMatrixIdentity(&matBone);

	memcpy(matBone.m[3], m_matRootBone.m[3], sizeof(_vec3));
	matBone._42 = 0.f;
	D3DXMatrixInverse(&matBone, nullptr, &matBone);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &(matBone * m_pTransCom->m_matWorld));
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

void COrge::Chase_Target()				// 타겟을 따라가는 함수.
{
	if (m_eCurState == CGameObject::DIE)
		return;

	_vec3 vTargetPos = m_pTarget->Get_TransformCom()->m_vInfo[Engine::INFO_POS];	//TargetPos
	_vec3 vTargetLook = vTargetPos - m_pTransCom->m_vInfo[Engine::INFO_POS];		// 타겟 위치 - 객체 위치
	_float fLength = D3DXVec3Length(&(vTargetLook));
	

	if (!m_bIsLookPlayer)			// 첫발견시.
	{
		_vec3 vLook = m_pTransCom->m_vInfo[Engine::INFO_LOOK] * m_pTransCom->m_vLook.z;			// z가 -1이기때문.
		D3DXVec3Normalize(&vLook, &vLook);
		D3DXVec3Normalize(&vTargetLook, &vTargetLook);

		_float fCos = D3DXToDegree(acosf(D3DXVec3Dot(&vLook, &vTargetLook)));

		if (fCos <= 10.f && 7.f >= fLength)
		{
			m_bIsLookPlayer = true;
			m_bIsDash = true;
			m_eCurState = CGameObject::ATTACK;
			m_eAttOption = RUSH;

			if (vTargetPos.z >= m_pTransCom->m_vInfo[Engine::INFO_POS].z)		// 플레이어가 문쪽에 있는 경우
				m_pTransCom->m_vAngle[Engine::ROT_Y] = D3DXToRadian(180.f);
			else
				m_pTransCom->m_vAngle[Engine::ROT_Y] = D3DXToRadian(0.f);
		}
		else
			m_eCurState = CGameObject::STAND;
	}
	else
	{
		if (!m_bIsDash)				// 대쉬중이라면 타면 안된다.
		{
			m_fBashTimer -= m_fTimeDelta;	//  해당 BashTimer가 0초가 되는 순간 걷는걸 멈추고 휘두른다.
			if (m_fBashTimer > 0.f)
			{
				m_eCurState = CGameObject::WALK;
				if (0.5f <= fLength)				// 너무 가까이 붙으면 그냥 제자리 걸음으로 대체.
				{
					m_pTransCom->Chase_Target_RotY(vTargetPos, Engine::ROT_Z);			// 타겟을 향해 방향을 틀어준다.
					m_pTransCom->m_vAngle[Engine::ROT_Y] *= -1.f;

					_vec3 vDir = m_pTransCom->Get_Dir(Engine::ROT_Z);		// 현재 matWorld가 업데이트 된 상황에서의 Look(메쉬가 바라보고있는방향마다 다름)
					_vec3 vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * m_fTimeDelta * -m_fSpeed));
					m_pTransCom->Set_Position(&vPos);
				}
			}
			else
			{
				m_eCurState = CGameObject::ATTACK;
				m_eAttOption = BASH;
			}
		}
	}

	//if (0.2f >= fLength)							// 거리가 0.2이하 일때 : 공격모션 취하기.
	//{
	//	m_eCurState = CGameObject::ATTACK;
	//	m_eAttOption = BASH;

	//	m_pTransCom->Chase_Target_RotY(vTargetPos, Engine::ROT_Z);			// 타겟을 향해 방향을 틀어준다.
	//}
	//else if (4.f >= fLength)						// 거리가 1.f이하 일때. : 쫓아오기.
	//{
	//	m_eCurState = CGameObject::ATTACK;
	//	m_eAttOption = RUSH;

	//	m_pTransCom->Chase_Target_RotY(vTargetPos, Engine::ROT_Z);			// 타겟을 향해 방향을 틀어준다.
	//	m_pTransCom->m_vAngle[Engine::ROT_Y] *= -1.f;
	//	//_vec3 vDir = m_pTransCom->Get_Dir(Engine::ROT_Z);		// 현재 matWorld가 업데이트 된 상황에서의 Look(메쉬가 바라보고있는방향마다 다름)
	//	//_vec3 vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vDir * m_fTimeDelta * -m_fSpeed));
	//	//m_pTransCom->Set_Position(&vPos);
	//}
	//else
	//{
	//	m_eCurState = CGameObject::STAND;
	//	m_eAttOption = BASH;
	//	m_bIsDash = false;
	//}

	if (m_eCurState == CGameObject::ATTACK)
	{
		switch (m_eAttOption)
		{
		case COrge::BASH:
			if (m_pMeshCom->Is_AnimationAfterTrack(3.0) && !m_pMeshCom->Is_AnimationAfterTrack(4.5))
			{
				Engine::Get_Layer(L"GameLogic")->CollisionObject(L"Player", m_matBone, m_fRadius);
				CSoundMgr::GetInstance()->MyPlaySound(L"orge_club_swing_a.wav", CSoundMgr::ORGE_BASH);
			}
			break;
		case COrge::RUSH:
			if (m_pMeshCom->Is_AnimationAfterTrack(8.0) && !m_pMeshCom->Is_AnimationAfterTrack(11.0))
			{
				_vec3 vLook = m_pTransCom->m_vInfo[Engine::INFO_LOOK];
				D3DXVec3Normalize(&vLook, &vLook);
				_float fX = m_pTransCom->m_vInfo[Engine::INFO_POS].x;
				_vec3 vPos = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->m_vInfo[Engine::INFO_POS], &(vLook * m_fTimeDelta * -m_fSpeed * 20.f));
				vPos.x = fX;
				m_pTransCom->Set_Position(&vPos);

				Engine::Get_Layer(L"GameLogic")->CollisionObject(L"Player", m_matBone, m_fRadius);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::ORGE_DASH1);
				CSoundMgr::GetInstance()->MyPlaySound(L"orge_jumpattack_02.wav", CSoundMgr::ORGE_DASH2);
			}
			else
				CSoundMgr::GetInstance()->MyPlaySound(L"orge_jumpattack_shout_a.wav", CSoundMgr::ORGE_DASH1);
			break;
		default:
			break;
		}
	}
}

void COrge::MotionChange()
{
	if (m_eCurState == CGameObject::DAMAGE)
		return;
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Engine::CGameObject::STAND:
			m_pMeshCom->Set_AnimationSet(2);
			m_pBoneName = "Bip01_Spine";
			m_fAniSpeed = 2.f;
			m_eAttOption = BASH;
			m_bIsDash = false;
			m_fBashTimer = 3.f;
			break;
		case Engine::CGameObject::WALK:
			m_pMeshCom->Set_AnimationSet(3);
			m_pBoneName = "Bip01_Spine";
			m_fAniSpeed = 2.f;

			break;
		case Engine::CGameObject::ATTACK:
			switch (m_eAttOption)
			{
			case COrge::BASH:
				m_iBashCount++;				// 카운트 증가.
				if (m_iBashCount >= 4)
				{
					m_iBashCount = 0;
					m_bIsLookPlayer = false;
					m_eCurState = CGameObject::STAND;
				}
				m_pMeshCom->Set_AnimationSet(0);
				m_fAniSpeed = 2.f;
				m_pBoneName = "Bip01_R_Hand";
				break;
			case COrge::RUSH:
				m_pMeshCom->Set_AnimationSet(8);
				m_fAniSpeed = 2.f;
				m_pBoneName = "Bip01_Spine";
				m_iBashCount = 0;
				break;
			}
			// 입쪽으로 바꿔야함.
			Engine::Get_Layer(L"GameLogic")->Reset_HitObject(L"Player");
			break;
		case Engine::CGameObject::DAMAGE:
			m_ePreState = m_eCurState;
			/*if (m_ePreState == CGameObject::ATTACK)
				break;
			switch (m_eHitOption)
			{
			case CGameObject::HIT_NORMAL:
				m_pMeshCom->Set_AnimationSet(1);
				m_fAniSpeed = 2.f;
				break;
			}*/
			m_pBoneName = "Bip01_Spine";
			break;
		case Engine::CGameObject::DIE:
			m_pMeshCom->Set_AnimationSet(9);
			m_pBoneName = "Bip01_Spine";
			m_fAniSpeed = 2.f;

			m_bIsDead = true;
			break;
		default:
			break;
		}
		m_matBone = m_pBoneColliderCom->Current_BoneMatrix(m_pBoneName);

		m_ePreState = m_eCurState;
	}
}

void COrge::MotionEvent()
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
		if (m_pMeshCom->Is_AnimationSetEnd())
			m_eCurState = CGameObject::STAND;
			//Engine::Get_Layer(L"GameLogic")->Reset_HitObject(L"Player");			//플레이어가 다시 맞게끔해야한다.
		break;
	case Engine::CGameObject::DAMAGE:
		break;
	case Engine::CGameObject::JUMP:
		break;
	case Engine::CGameObject::DIE:
		break;
	default:
		break;
	}
}

COrge* COrge::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh, CPlayer* pPlayer)
{
	COrge* pInstance = new COrge(pGraphicDev, pPlayer);


	if (FAILED(pInstance->Ready_Object(0)))
	{
		ERR_BOX("Orge Created Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->Set_NaviCom(pNaviMesh);					// Terrain의 NaviMesh를 세팅한다

	return pInstance;
}

COrge * COrge::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh * pNaviMesh, CPlayer* pPlayer,
	const _vec3 & vPos, const _vec3 & vSize, const _vec3 & vRotate, const _uint& iGroup)
{
	COrge* pInstance = new COrge(pGraphicDev, pPlayer);

	if (FAILED(pInstance->Ready_Object(0)))
	{
		ERR_BOX("Orge Created Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->Set_Pos(vPos);
	pInstance->Set_Size(vSize);
	pInstance->Set_Rotate(vRotate);
	pInstance->Set_NaviCom(pNaviMesh);					// Terrain의 NaviMesh를 세팅한다
	pInstance->Set_Group(iGroup);
	return pInstance;
}

void COrge::Free(void)
{
	if(nullptr != m_pSword)
		Engine::Safe_Release(m_pSword);
	Engine::CGameObject::Free();
}