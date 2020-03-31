#include "stdafx.h"
#include "Effect.h"

#include "Export_Function.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_fFrame(0.f)
{

}

CEffect::~CEffect(void)
{

}

HRESULT CEffect::Ready_Object(void)
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransCom->Set_Pos(&_vec3(_float(rand() % 20), 1.f, _float(rand() % 20)));


	return S_OK;
}

_int CEffect::Update_Object(const _float& fTimeDelta)
{
	m_fFrame += 90.f * fTimeDelta;

	if (90.f < m_fFrame)
		m_fFrame = 0.f;

	Engine::CGameObject::Update_Object(fTimeDelta);

	_matrix		matView, matBill, matWorld;

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, NULL, &matBill);

	m_pTransCom->m_matWorld = matBill * matWorld;

	Compute_ViewZ(&m_pTransCom->m_vInfo[Engine::INFO_POS]);

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	return 0;
}

void CEffect::Render_Object(void)
{

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransCom->m_matWorld);

	m_pTextureCom->Render(_uint(m_fFrame));

	m_pBufferCom->Render_Buffer();
}

void CEffect::Free(void)
{
	Engine::CGameObject::Free();
}

HRESULT CEffect::AddComponent(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer Component
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::Clone_Resources(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::Clone_Resources(RESOURCE_STAGE, L"Texture_Explosion"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Renderer
	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}

CEffect* CEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect*		pInstance = new CEffect(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		ERR_BOX("Effect Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

