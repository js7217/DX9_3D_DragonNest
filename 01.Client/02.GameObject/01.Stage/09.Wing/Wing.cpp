#include "stdafx.h"
#include "Wing.h"

#include "Export_Function.h"

CWing::CWing(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_wstrName(L"Wing")
	, m_bIsDead(false)
	, m_pParentBoneMatrix(nullptr)
	, m_pParentWorldMatrix(nullptr)
	, m_eMode(DISPLAY)
{

}

CWing::~CWing(void)
{

}

const _vec3 & CWing::Get_Pos()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *(m_pTransCom->Get_Info(Engine::INFO_POS));
}

const _vec3 & CWing::Get_Size()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_pTransCom->m_vScale;
}

const _vec3 & CWing::Get_Rotate()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_pTransCom->m_vAngle;
}

void CWing::Set_Pos(const _vec3 & vPos)
{
	m_pTransCom->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CWing::Set_Size(const _vec3 & vSize)
{
	m_pTransCom->Set_Scale(vSize.x, vSize.y, vSize.z);
}

void CWing::Set_Rotate(const _vec3 & vRotate)
{
	m_pTransCom->m_vAngle = vRotate;
}

HRESULT CWing::Ready_Object()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXMatrixIdentity(&m_pTransCom->m_matWorld);

	return S_OK;
}

_int CWing::Update_Object(const _float& fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	if (EQUIP == m_eMode && nullptr == m_pParentBoneMatrix)
	{
		Set_Size(_vec3(0.05f, 0.05f, 0.05f));
		Set_Rotate(_vec3(D3DXToRadian(0.f), D3DXToRadian(180.f), D3DXToRadian(90.f))); // 원래 것 사이즈 변경...

		Engine::CDynamicMesh*		pPlayerMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Mesh", Engine::ID_STATIC));
		if (nullptr == pPlayerMeshCom)
			return 0;
		const Engine::D3DXFRAME_DERIVED*	pFrame = pPlayerMeshCom->Get_FrameByName("Bip01_Spine");

		m_pParentBoneMatrix = &pFrame->CombinedTransformMatrix;

		Engine::CTransform*		pPlayerTransCom = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));
		if (nullptr == pPlayerTransCom)
			return 0;

		m_pParentWorldMatrix = pPlayerTransCom->Get_WorldMatrixPointer();
	}

	Engine::CGameObject::Update_Object(fTimeDelta);

	Change_WingMode();
	if (EQUIP == m_eMode)
	{
		if(nullptr != m_pParentBoneMatrix)
			m_pTransCom->Set_ParentMatrix((*m_pParentBoneMatrix * *m_pParentWorldMatrix));
	}


	m_pMeshCom->Play_AnimationSet(fTimeDelta);
	m_pMeshCom->Set_AnimationSet(m_eMode);

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	return 0;
}

void CWing::Render_Object(void)
{
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

HRESULT CWing::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_Wing"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CWing::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

void CWing::Change_WingMode()
{
	_bool bIsMode = Engine::Get_Layer(L"GameLogic")->Empty_GameObject(L"TinyDragon");

	if (bIsMode)
		m_eMode = EQUIP;
	else
		m_eMode = DISPLAY;
}

CWing* CWing::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWing* pInstance = new CWing(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		ERR_BOX("Wing Created Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

CWing * CWing::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 & vPos, const _vec3 & vSize, const _vec3 & vRotate)
{
	CWing* pInstance = new CWing(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		ERR_BOX("Wing Created Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->Set_Pos(vPos);
	pInstance->Set_Size(vSize);
	pInstance->Set_Rotate(vRotate);

	return pInstance;
}

void CWing::Free(void)
{
	Engine::CGameObject::Free();
}
