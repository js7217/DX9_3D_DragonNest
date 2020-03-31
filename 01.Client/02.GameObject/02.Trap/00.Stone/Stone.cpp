#include "stdafx.h"
#include "Stone.h"

#include "Export_Function.h"
#include "Dust.h"
CStone::CStone(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_vDir(0.f, 0.f, 0.f)
	, m_fSpeed(10.f), m_fTimer(3.0f)
{

}

CStone::~CStone(void)
{

}

HRESULT CStone::Ready_Object(const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_float fPosZ = Random(-0.25f, 0.25f);
	m_pTransCom->m_vAngle = vRotate;
	m_pTransCom->m_vScale = vSize;
	m_pTransCom->m_vInfo[Engine::INFO_POS] = _vec3(vPos.x, vPos.y, vPos.z + fPosZ);

	m_fRadius = 0.3f;
	m_fSpeed = 5.f;
	CSoundMgr::GetInstance()->StopSound(CSoundMgr::ROCK_ROLLING);
	CSoundMgr::GetInstance()->MyPlaySound(L"ox_roll_loop_b.ogg", CSoundMgr::ROCK_ROLLING);
	return S_OK;
}

_int CStone::Update_Object(const _float& fTimeDelta)
{
	if (m_bIsDead)
		return 1;
	m_fTimeDelta = fTimeDelta;

	Engine::CGameObject::Update_Object(fTimeDelta);
	Go_Straight(fTimeDelta);

	m_matBone = m_pTransCom->m_matWorld;

	Engine::Get_Layer(L"GameLogic")->CollisionObject(L"Player", m_matBone, m_fRadius, CGameObject::HIT_KNOCKBACK);	// 매프레임 플레이어와 충돌체크.

	_vec3 vPos = {};
	m_pTransCom->Get_Info(Engine::INFO_POS, &vPos);
	m_bDraw = m_pOptimization->IsInFrustum_ForObject(&vPos, 2.f);
	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	m_fTimer -= m_fTimeDelta;
	m_fDustTimer -= m_fTimeDelta;
	if (m_fTimer <= 0.f)
		m_bIsDead = true;

	if (m_fDustTimer <= 0.f)
	{
		Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"Dust", CDust::Create(m_pGraphicDev, *m_pTransCom->Get_Info(Engine::INFO_POS), CDust::ROLLING));
		m_fDustTimer = 0.3f;
	}

	return 0;
}

void CStone::Render_Object(void)
{
	if (false == m_bDraw)
		return;

	m_pMeshCom->Play_AnimationSet(m_fTimeDelta * 5.f); // 순서 유의..?

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
}

HRESULT CStone::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pTransCom = Engine::CTransform::Create(_vec3(0.f, 0.f, -1.f));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);
	
	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_Stone"));
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

HRESULT CStone::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

void CStone::Go_Straight(const _float& fTimeDelta)
{
	_vec3 vLook = m_pTransCom->m_vInfo[Engine::INFO_LOOK] * m_pTransCom->m_vLook.z;
	D3DXVec3Normalize(&vLook, &vLook);

	m_pTransCom->Set_Pos(&(vLook * m_fSpeed * fTimeDelta));
}

float CStone::Random(float min, float max)
{
	random_device rn;
	mt19937_64 rnd(rn());
	uniform_real_distribution<float> range(min, max);

	return range(rnd);
}


CStone* CStone::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate)
{
	CStone* pInstance = new CStone(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(vPos, vSize, vRotate)))
	{
		ERR_BOX("Stone Created Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CStone::Free(void)
{
	Engine::CGameObject::Free();
}
