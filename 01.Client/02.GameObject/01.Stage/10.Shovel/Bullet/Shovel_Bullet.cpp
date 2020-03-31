#include "stdafx.h"
#include "Shovel_Bullet.h"

#include "Export_Function.h"

CShovel_Bullet::CShovel_Bullet(LPDIRECT3DDEVICE9 pGarphicDev)
	: CGameObject(pGarphicDev)
{
}

CShovel_Bullet::~CShovel_Bullet(void)
{
}


HRESULT CShovel_Bullet::Ready_Object(const _vec3& vThrowLook, const _vec3& vStartPos, const _float& fRotY)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_vThrowLook = vThrowLook;
	D3DXVec3Normalize(&m_vThrowLook, &m_vThrowLook);

	m_pTransCom->Set_Pos(vStartPos.x, 0.2f, vStartPos.z);
	m_pTransCom->Set_Scale(0.005f, 0.005f, 0.005f);
	m_pTransCom->m_vAngle.y = fRotY;

	m_fRadius = 0.03f;

	return S_OK;
}

_int CShovel_Bullet::Update_Object(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	Engine::CGameObject::Update_Object(fTimeDelta);

	m_matBone = m_pTransCom->m_matWorld;

	m_fThrowTimer -= fTimeDelta;
	if (m_fThrowTimer <= 0.f)			// 시간제한이 끝나면 돌아와라.
	{
		m_bIsDead = true;
	}

	m_pTransCom->Set_Pos(&(m_vThrowLook * fTimeDelta * 5.f));

	if (Engine::Get_Layer(L"GameLogic")->CollisionObject(L"Player", m_matBone, m_fRadius))
		m_bIsDead = true;

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	return 0;
}

void CShovel_Bullet::Render_Object(void)
{
	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
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

HRESULT CShovel_Bullet::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;


	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_Spartan_Weapon"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CShovel_Bullet::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CShovel_Bullet * CShovel_Bullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vThrowLook, const _vec3& vStartPos, const _float& fRotY)
{
	CShovel_Bullet* pInstance = new CShovel_Bullet(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(vThrowLook, vStartPos, fRotY)))
	{
		ERR_BOX("Shovel_Bullet Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CShovel_Bullet::Free(void)
{
	Engine::CGameObject::Free();
}
