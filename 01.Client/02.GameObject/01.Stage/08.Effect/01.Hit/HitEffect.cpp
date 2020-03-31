#include "stdafx.h"
#include "HitEffect.h"

#include "Export_Function.h"

CHitEffect::CHitEffect(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pResourceName)
	: Engine::CGameObject(pGraphicDev)
	, m_bIsDead(false)
	, m_fLifeTime(0.5f), m_fAlpha(1.f), m_fAddPosY(0.f)
	, m_pResourceName(pResourceName)
{

}

CHitEffect::~CHitEffect(void)
{

}

void CHitEffect::Set_Pos(const _vec3 & vPos)
{
	if (nullptr == m_pTransCom)
		return;

	m_pTransCom->Set_Pos(vPos.x, vPos.y + m_fAddPosY, vPos.z);
}

HRESULT CHitEffect::Ready_Object(void)
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);
	m_pTransCom->Set_Scale(0.5f, 0.5f, 0.5f);

	return S_OK;
}

_int CHitEffect::Update_Object(const _float& fTimeDelta)
{
	if (m_fAlpha <= 0.f)
		return 1;			// 소멸

	m_fAlpha -= fTimeDelta / m_fLifeTime;		// 알파값.

	Update_Option(fTimeDelta);
	
	Engine::CGameObject::Update_Object(fTimeDelta);

	Set_BillBoard();

	Compute_ViewZ(&m_pTransCom->m_vInfo[Engine::INFO_POS]);

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	return 0;
}

void CHitEffect::Render_Object(void)
{
	if (nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	pEffect->AddRef();

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

void CHitEffect::Free(void)
{
	Engine::CGameObject::Free();
}

HRESULT CHitEffect::AddComponent(void)
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
		(Engine::Clone_Resources(RESOURCE_STAGE, m_pResourceName));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Renderer
	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_MeshEffect"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CHitEffect::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &m_pTransCom->m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	Safe_Release(pEffect);

	return S_OK;
}


HRESULT CHitEffect::Update_Option(const _float& fTimeDelta)
{
	_float fScale = m_fLifeTime * fTimeDelta;
	m_pTransCom->m_vScale += _vec3(fScale, fScale, fScale);

	m_pTransCom->m_vInfo[Engine::INFO_POS].y += m_fAddPosY * fTimeDelta * 0.5f;

	return NOERROR;
}

HRESULT CHitEffect::Set_BillBoard()
{
	_matrix		matView, matBill, matWorld;

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, NULL, &matBill);				// y축 회전.

	m_pTransCom->m_matWorld = matBill * matWorld;

	return NOERROR;
}

CHitEffect* CHitEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _tchar* pResourceName)
{
	CHitEffect*		pInstance = new CHitEffect(pGraphicDev, pResourceName);

	if (FAILED(pInstance->Ready_Object()))
	{
		ERR_BOX("HitEffect Create Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->Set_Pos(vPos);

	return pInstance;
}