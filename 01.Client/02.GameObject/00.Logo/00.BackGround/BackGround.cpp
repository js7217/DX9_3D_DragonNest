#include "stdafx.h"
#include "BackGround.h"

#include "Export_Function.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTextureCom(nullptr)
	, m_pTransCom(nullptr), m_pRenderCom(nullptr)
{

}

CBackGround::~CBackGround(void)
{

}

HRESULT CBackGround::Ready_Object(void)
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransCom->m_vScale = _vec3(2.f, 2.f, 2.f);

	return S_OK;
}

_int CBackGround::Update_Object(const _float& fTimeDelta)
{
//	Key_Input(fTimeDelta);
	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_PRIORITY, this);

	return 0;
}

void CBackGround::Render_Object(void)
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);

	_uint iPassCnt = 0;
	_matrix matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &m_pTransCom->m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	// 1번째 인자 : OutPut, 현재 쉐이더 코드가 지닌 최대 패스갯수. 2인자 : 시작하는 방식 플래그
	pEffect->Begin(&iPassCnt, 0);
//	pEffect->BeginPass(0); // 내가 구동하고자 하는 쉐이더의 PASS

	pEffect->BeginPass(1); // 내가 구동하고자 하는 쉐이더의 PASS

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();
}

void CBackGround::Free(void)
{
	Engine::CGameObject::Free();
}

HRESULT CBackGround::AddComponent(void)
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
		(Engine::Clone_Resources(RESOURCE_LOGO, L"Texture_Back"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Renderer
	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_Sample"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

void CBackGround::Key_Input(const _float & fTimeDelta)
{
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_pTransCom->Rotation(Engine::ROT_X, D3DXToRadian(90.f * fTimeDelta));
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_pTransCom->Rotation(Engine::ROT_X, D3DXToRadian(-90.f * fTimeDelta));
	}

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_pTransCom->Rotation(Engine::ROT_Y, D3DXToRadian(90.f * fTimeDelta));
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_pTransCom->Rotation(Engine::ROT_Y, D3DXToRadian(-90.f * fTimeDelta));
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		m_pTransCom->Rotation(Engine::ROT_Z, D3DXToRadian(90.f * fTimeDelta));
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_pTransCom->Rotation(Engine::ROT_Z, D3DXToRadian(-90.f * fTimeDelta));
	}
}

CBackGround* CBackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBackGround*		pInstance = new CBackGround(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		ERR_BOX("BackGround Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

