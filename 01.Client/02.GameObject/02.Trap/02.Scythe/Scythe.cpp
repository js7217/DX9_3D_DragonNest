#include "stdafx.h"
#include "Scythe.h"

#include "Export_Function.h"

CScythe::CScythe(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_fSpeed(10.f)
{

}

CScythe::~CScythe(void)
{

}

HRESULT CScythe::Ready_Object(const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->m_vAngle = vRotate;
	m_pTransCom->m_vScale = vSize;
	m_pTransCom->m_vInfo[Engine::INFO_POS] = _vec3(vPos.x, vPos.y, vPos.z);

	m_fRadius = 0.2f;
	m_fSpeed = Random(1.f, 2.f);

	m_pBoneName = "_Scythe";

	const Engine::D3DXFRAME_DERIVED* pFrame = m_pMeshCom->Get_FrameByName(m_pBoneName);
	m_pParentBoneMatrix = &pFrame->CombinedTransformMatrix;

	return S_OK;
}

_int CScythe::Update_Object(const _float& fTimeDelta)
{
	if (!m_bIsOpen)
		return 0;

	m_fTimeDelta = fTimeDelta;
	Engine::CGameObject::Update_Object(fTimeDelta);

	//_vec3 vBoneUp = {};
	//memcpy(&vBoneUp, m_pParentBoneMatrix->m[1], sizeof(_vec3));		//  돌렸을때 나오는 Up벡터의 값.
	//D3DXVec3Normalize(&vBoneUp, &vBoneUp);
	//vBoneUp *= m_fLength;

	//D3DXMatrixTranslation(&m_matDist, vBoneUp.x, vBoneUp.y, vBoneUp.z);	// 아래방향으로 할것이다.


	//m_matBone = *m_pParentBoneMatrix * (m_pTransCom->m_matWorld * m_matDist);

	Engine::Get_Layer(L"GameLogic")->CollisionObject(L"Player", m_matBone, m_fRadius, CGameObject::HIT_KNOCKBACK);	// 매프레임 플레이어와 충돌체크.

	_vec3 vPos = {};
	m_pTransCom->Get_Info(Engine::INFO_POS, &vPos);
	m_bDraw = m_pOptimization->IsInFrustum_ForObject(&vPos, 2.f);
	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	return 0;
}

void CScythe::Render_Object(void)
{
	if (false == m_bDraw || !m_bIsOpen)
		return;

	m_pMeshCom->Play_AnimationSet(m_fTimeDelta * m_fSpeed); // 순서 유의..?

	const Engine::D3DXFRAME_DERIVED* pFrame = m_pMeshCom->Get_FrameByName(m_pBoneName);
	m_pParentBoneMatrix = &pFrame->CombinedTransformMatrix;

	_matrix matTrans;
	D3DXMatrixTranslation(&matTrans, -780.f, 0.f, 0.f);

	m_matBone = matTrans * *m_pParentBoneMatrix * m_pTransCom->m_matWorld;	// 본의 월드값.


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

	//LPD3DXMESH pSphere;
	//D3DXCreateSphere(m_pGraphicDev, 50.f, 10, 10, &pSphere, NULL); // 반지름은 10(로컬)

	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matBone);
	//pSphere->DrawSubset(0);

	//Safe_Release(pSphere);
}

HRESULT CScythe::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pTransCom = Engine::CTransform::Create(_vec3(0.f, 0.f, -1.f));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);
	
	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_scythe"));
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

HRESULT CScythe::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

float CScythe::Random(float min, float max)
{
	random_device rn;
	mt19937_64 rnd(rn());
	uniform_real_distribution<float> range(min, max);

	return range(rnd);
}


CScythe* CScythe::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate)
{
	CScythe* pInstance = new CScythe(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(vPos, vSize, vRotate)))
	{
		ERR_BOX("Scythe Created Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CScythe::Free(void)
{
	Engine::CGameObject::Free();
}
