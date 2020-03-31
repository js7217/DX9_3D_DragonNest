#include "stdafx.h"
#include "Tree.h"

#include "Export_Function.h"

CTree::CTree(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_vDir(0.f, 0.f, 0.f)
{

}

CTree::~CTree(void)
{

}

HRESULT CTree::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fSpeed = 10.f;
	m_iFlag = iFlag;

	m_pTransCom->Set_Scale(0.01f, 0.01f, 0.01f);
	m_pTransCom->Set_Pos(_float(rand() % 10), 0.f, _float(rand() % 10));
	m_pTransCom->Rotation(Engine::ROT_Y, D3DXToRadian(45.f));

	return S_OK;
}

_int CTree::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);

	Set_OnTerrain();

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

void CTree::Render_Object(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	pEffect->Begin(NULL, 0);
	
	m_pMeshCom->Render_Meshes(pEffect);

	pEffect->End();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	Safe_Release(pEffect);
}

void CTree::Free(void)
{
	Engine::CGameObject::Free();
}

HRESULT CTree::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);


	// Renderer
	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_Tree"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Collider
	pComponent = m_pColliderCom = Engine::CBoxCollider::Create(m_pGraphicDev, 
		m_pMeshCom->Get_VtxMeshPos(),
		m_pMeshCom->Get_NumVtx(), 
		m_pMeshCom->Get_Stride());
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CTree::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &m_pTransCom->m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	Safe_Release(pEffect);

	return S_OK;
}

CTree* CTree::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CTree*	pInstance = new CTree(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
	{
		ERR_BOX("Tree Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}



void CTree::Set_OnTerrain(void)
{
	_vec3		vPos;
	m_pTransCom->Get_Info(Engine::INFO_POS, &vPos);

	Engine::CTerrainTex*	pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>
		(Engine::Get_Component(L"GameLogic", 
								L"Terrain", 
								L"Com_Buffer",
								Engine::ID_STATIC));

	if (nullptr == pTerrainBufferCom)
		return;

	_float	fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos(), 129, 129);

	m_pTransCom->Set_Pos(vPos.x, fHeight, vPos.z);


}


_bool CTree::Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag)
{
	Engine::CBoxCollider*	pPlayerColliderCom = dynamic_cast<Engine::CBoxCollider*>(Engine::Get_Component(pLayerTag, pObjTag, L"Com_Collider", Engine::ID_STATIC));


	if (0 == m_iFlag)
	{
		return m_pCalculatorCom->Collision_AABB(pPlayerColliderCom->Get_Min(), pPlayerColliderCom->Get_Max(), pPlayerColliderCom->Get_ColWorld(),
								    			m_pColliderCom->Get_Min(), m_pColliderCom->Get_Max(), m_pColliderCom->Get_ColWorld());
	}
	else
	{
		return m_pCalculatorCom->Collision_OBB(pPlayerColliderCom->Get_Min(), pPlayerColliderCom->Get_Max(), pPlayerColliderCom->Get_ColWorld(),
												m_pColliderCom->Get_Min(), m_pColliderCom->Get_Max(), m_pColliderCom->Get_ColWorld());
	}
	
}

