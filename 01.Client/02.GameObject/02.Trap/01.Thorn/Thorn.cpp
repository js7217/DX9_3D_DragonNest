#include "stdafx.h"
#include "Thorn.h"

#include "Export_Function.h"

CThorn::CThorn(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_fSpeed(10.f)
{

}

CThorn::~CThorn(void)
{

}

HRESULT CThorn::Ready_Object(const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->m_vAngle = vRotate;
	m_pTransCom->m_vScale = vSize;
	m_pTransCom->m_vInfo[Engine::INFO_POS] = vPos;

	m_fRadius = 0.2f;
	m_fSpeed = 1.f;

	const Engine::D3DXFRAME_DERIVED*	pFrame = m_pMeshCom->Get_FrameByName("Bone05");
	m_pParentBoneWorld = &(pFrame->CombinedTransformMatrix);

	m_pBoneName = "Bone05";
	_uint iNum = (_uint)Random(1.1f, 2.9f);
	m_eCurState = (CGameObject::STATE)iNum;
	m_pMeshCom->Set_AnimationSet(iNum);
	return S_OK;
}

_int CThorn::Update_Object(const _float& fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	Engine::CGameObject::Update_Object(fTimeDelta);

	MotionChange();
	MotionEvent();


	_vec3 vPos = {};
	m_pTransCom->Get_Info(Engine::INFO_POS, &vPos);
	m_bDraw = m_pOptimization->IsInFrustum_ForObject(&vPos, 2.f);
	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

void CThorn::Render_Object(void)
{
	m_pMeshCom->Play_AnimationSet(m_fTimeDelta * m_fSpeed); // 순서 유의..?

	if (!m_bDraw)
		return;
	LPD3DXEFFECT		pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	const Engine::D3DXFRAME_DERIVED* pFrame = m_pMeshCom->Get_FrameByName(m_pBoneName);
	_matrix matBoneMatrix = pFrame->CombinedTransformMatrix;

	m_matBone = matBoneMatrix * m_pTransCom->m_matWorld;

	pEffect->AddRef();

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	m_pMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	//LPD3DXMESH pSphere;
	//D3DXCreateSphere(m_pGraphicDev, 50.f, 10, 10, &pSphere, NULL); // 반지름은 10(로컬)

	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matBone);
	//pSphere->DrawSubset(0);

	//Safe_Release(pSphere);
}

HRESULT CThorn::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pTransCom = Engine::CTransform::Create(_vec3(0.f, 0.f, -1.f));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);
	
	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_Trap1"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Optimization
	pComponent = m_pOptimization = dynamic_cast<Engine::COptimization*>
		(Engine::Clone(L"Proto_Optimization"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Optimization", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CThorn::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

float CThorn::Random(float min, float max)
{
	random_device rn;
	mt19937_64 rnd(rn());
	uniform_real_distribution<float> range(min, max);

	return range(rnd);
}

void CThorn::MotionChange()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Engine::CGameObject::STAND:			// 0번 : 내리기
			m_pMeshCom->Set_AnimationSet(0);
			break;
		case Engine::CGameObject::WALK:				// 1번 : 온스탠드
			m_pMeshCom->Set_AnimationSet(1);
			break;
		case Engine::CGameObject::ATTACK:			// 2번 : 다운 스탠드
			m_pMeshCom->Set_AnimationSet(2);
			break;
		case Engine::CGameObject::JUMP:				// 3번 : 올리기
			m_pMeshCom->Set_AnimationSet(3);
			Engine::Get_Layer(L"GameLogic")->Reset_HitObject(L"Player");

			break;
		default:
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CThorn::MotionEvent()
{
	switch (m_eCurState)
	{
	case Engine::CGameObject::STAND:
		if (m_pMeshCom->Is_AnimationSetEnd())
			m_eCurState = ATTACK;
		break;
	case Engine::CGameObject::WALK:
		m_fTimer -= m_fTimeDelta;
		Engine::Get_Layer(L"GameLogic")->CollisionObject(L"Player", m_matBone, m_fRadius);
		if (m_fTimer <= 0.f)
		{
			m_eCurState = CGameObject::STAND;
			m_fTimer = 2.f;
		}
		break;
	case Engine::CGameObject::ATTACK:
		m_fTimer -= m_fTimeDelta;
		if (m_fTimer <= 0.f)
		{
			m_eCurState = CGameObject::JUMP;
			m_fTimer = 2.f;
		}
		break;
	case Engine::CGameObject::JUMP:
		Engine::Get_Layer(L"GameLogic")->CollisionObject(L"Player", m_matBone, m_fRadius);
		if (m_pMeshCom->Is_AnimationSetEnd(0.2))
			m_eCurState = WALK;
		break;
	default:
		break;
	}
}


CThorn* CThorn::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate)
{
	CThorn* pInstance = new CThorn(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(vPos, vSize, vRotate)))
	{
		ERR_BOX("Thorn Created Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CThorn::Free(void)
{
	Engine::CGameObject::Free();
}
