#include "stdafx.h"
#include "Terrain.h"

#include "Export_Function.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTextureCom(nullptr)
	, m_pTransCom(nullptr), m_pRenderCom(nullptr)
{

}

CTerrain::~CTerrain(void)
{
}

void CTerrain::Get_NaviMesh(Engine::CNaviMesh ** ppNaviMesh)
{
	*ppNaviMesh = m_pNaviCom;
}

HRESULT CTerrain::Ready_Object(void)
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pIndex = new Engine::INDEX32[m_pBufferCom->Get_TriCnt()];

//	m_pTransCom->m_vScale = _vec3(2.f, 2.f, 2.f);

	return S_OK;
}

_int CTerrain::Update_Object(const _float& fTimeDelta)
{
//	Key_Input(fTimeDelta);
	Engine::CGameObject::Update_Object(fTimeDelta);

	//m_pOptimizationCom->IsInFrustum_ForTerrain(m_pBufferCom->Get_VtxPos(),
	//	m_pBufferCom->Get_VtxCntX(), m_pBufferCom->Get_VtxCntZ(),
	//	m_pIndex, &m_dwTriCnt);

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

void CTerrain::Render_Object(void)
{
//	m_pBufferCom->Copy_Index(m_pIndex, m_dwTriCnt);

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();

	FAILED_CHECK_RETURN(Set_ConStantTable(pEffect), );

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if(GetAsyncKeyState(VK_TAB) & 0x8000)
		m_pNaviCom->Render_NaviMesh();
}

void CTerrain::Create_Cell(const _vec3 & vPos, const Engine::CCell::CELL_OPTION & eOption)
{
	m_pNaviCom->Set_CellPos(vPos, eOption); // 꼭지점 생성
}

void CTerrain::Link_Cell()
{
	m_pNaviCom->Link_Cell();
}

void CTerrain::Free(void)
{
	Engine::CGameObject::Free();

	Engine::Safe_Delete_Array(m_pIndex);
}

HRESULT CTerrain::AddComponent(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer Component
	pComponent = m_pBufferCom = dynamic_cast<Engine::CTerrainTex*>
		(Engine::Clone_Resources(RESOURCE_STATIC, L"Buffer_TerrainTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	
	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::Clone_Resources(RESOURCE_STAGE, L"Texture_Terrain"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Filter Texture
	pComponent = m_pFilterCom = dynamic_cast<Engine::CTexture*>
		(Engine::Clone_Resources(RESOURCE_STAGE, L"Texture_Filter"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Filter", pComponent);

	// Brush Texture
	pComponent = m_pBrushCom = dynamic_cast<Engine::CTexture*>
		(Engine::Clone_Resources(RESOURCE_STAGE, L"Texture_Brush"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Brush", pComponent);

	// Renderer
	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	//// Optimization
	//pComponent = m_pOptimizationCom = dynamic_cast<Engine::COptimization*>
	//	(Engine::Clone(L"Proto_Optimization"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Optimization", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_Terrain"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);
	
	// Navi Com
	pComponent = m_pNaviCom = dynamic_cast<Engine::CNaviMesh*>
		(Engine::Clone(L"Proto_NaviMesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);

	/*pComponent = m_pNaviCom = Engine::CNaviMesh::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);*/

	return S_OK;
}

HRESULT CTerrain::Set_ConStantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &m_pTransCom->m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture"); // 텍스쳐 1번


	pEffect->SetFloat("g_fDetail", 20.f);

	Safe_Release(pEffect);

	return S_OK;
}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain*		pInstance = new CTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		ERR_BOX("Terrain Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

