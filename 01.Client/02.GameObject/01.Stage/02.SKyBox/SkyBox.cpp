#include "stdafx.h"
#include "SkyBox.h"

#include "Export_Function.h"

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTextureCom(nullptr)
	, m_pTransCom(nullptr), m_pRenderCom(nullptr)
{
	
}

CSkyBox::~CSkyBox(void)
{

}

HRESULT CSkyBox::Ready_Object(void)
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransCom->Set_Scale(40.f, 40.f, 40.f);

	return S_OK;
}

_int CSkyBox::Update_Object(const _float& fTimeDelta)
{
//	Key_Input(fTimeDelta);
	_matrix matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixInverse(&matView, NULL, &matView);

	m_pTransCom->Set_Pos(matView._41, matView._42, matView._43);

	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_PRIORITY, this);

	return 0;
}

void CSkyBox::Render_Object(void)
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);

	pEffect->AddRef();

	FAILED_CHECK_RETURN(Set_ConstantTable(pEffect), );

	// 1번째 인자 : OutPut, 현재 쉐이더 코드가 지닌 최대 패스갯수. 2인자 : 시작하는 방식 플래그
	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0); // 내가 구동하고자 하는 쉐이더의 PASS

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

void CSkyBox::Free(void)
{
	Engine::CGameObject::Free();
}

HRESULT CSkyBox::AddComponent(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer Component
	pComponent = m_pBufferCom = dynamic_cast<Engine::CCubeTex*>
		(Engine::Clone_Resources(RESOURCE_STATIC, L"Buffer_CubeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	
	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::Clone_Resources(RESOURCE_STAGE, L"Texture_SkyBox"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Renderer
	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_SkyBox"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CSkyBox::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return E_FAIL;

	pEffect->AddRef();

	_matrix matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &m_pTransCom->m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", 2);

	Safe_Release(pEffect);
	return NOERROR;
}

CSkyBox* CSkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkyBox*		pInstance = new CSkyBox(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		ERR_BOX("SkyBox Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

