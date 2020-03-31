#include "Renderer.h"
#include "Shader.h"
#include "Target_Manager.h"
#include "LightMgr.h"

USING(Engine)

Engine::CRenderer::CRenderer(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
{
	m_pGraphicDev->AddRef();
	m_pTarget_Manager->AddRef();
}

CRenderer::~CRenderer(void)
{
}

HRESULT CRenderer::Clear_Renderer()
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderGroup[i])
		{
			Safe_Release(pGameObject);
		}
		m_RenderGroup[i].clear();
	}
	return S_OK;
}

HRESULT CRenderer::Ready_Component()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	D3DVIEWPORT9		ViewPort;

	m_pGraphicDev->GetViewport(&ViewPort);

	// For.Target_Diffuse
	if (FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Diffuse", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For.Target_Normal
	if (FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Normal", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Depth
	if (FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Depth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Shadow
	if (FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Shadow", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Shade
	if (FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Shade", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Specular
	if (FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Specular", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;



	// For.MRT_Deferred
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Depth")))
		return E_FAIL;

	// For.MRT_LightAcc
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Specular")))
		return E_FAIL;

	// For.MRT_Shadow
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Shadow", L"Target_Shadow")))
		return E_FAIL;

	// For.Shader_LightAcc
	m_pShader_LightAcc = CShader::Create(m_pGraphicDev, L"../../Reference/Header/Shader_LightAcc.hpp");
	if (nullptr == m_pShader_LightAcc)
		return E_FAIL;

	// For.Shader_Blend
	m_pShader_Blend = CShader::Create(m_pGraphicDev, L"../../Reference/Header/Shader_Blend.hpp");
	if (nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pGraphicDev->CreateVertexBuffer(sizeof(VTXSCREEN) * 4, 0, FVF_SCREEN, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXSCREEN*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPos = _vec4(0.f - 0.5f, 0.f - 0.5f, 0.0f, 1.f);
	pVertices[0].vTexUV = _vec2(0.0f, 0.f);

	pVertices[1].vPos = _vec4(ViewPort.Width - 0.5f, 0.0f - 0.5f, 0.0f, 1.f);
	pVertices[1].vTexUV = _vec2(1.0f, 0.f);

	pVertices[2].vPos = _vec4(ViewPort.Width - 0.5f, ViewPort.Height - 0.5f, 0.0f, 1.f);
	pVertices[2].vTexUV = _vec2(1.0f, 1.f);

	pVertices[3].vPos = _vec4(0.0f - 0.5f, ViewPort.Height - 0.5f, 0.0f, 1.f);
	pVertices[3].vTexUV = _vec2(0.0f, 1.f);

	m_pVB->Unlock();

	if (FAILED(m_pGraphicDev->CreateIndexBuffer(sizeof(INDEX16) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	INDEX16*		pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();


#ifdef _DEBUG
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Diffuse", 0.f, 0.f, 100.f, 100.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Normal", 0.f, 100.f, 100.f, 100.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Depth", 0.f, 200.f, 100.f, 100.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shade", 100.f, 0.f, 100.f, 100.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Specular", 100.f, 100.f, 100.f, 100.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shadow", 700.f, 0.f, 100.f, 100.f);
#endif

	return NOERROR;
}

void Engine::CRenderer::Add_RenderGroup(RENDERID eGroup, CGameObject* pGameObject)
{
	if (nullptr == pGameObject || RENDER_END <= eGroup)
		return;

	m_RenderGroup[eGroup].emplace_back(pGameObject);
	pGameObject->AddRef();
}

void Engine::CRenderer::Render_GameObject()
{
	Render_Priority();

	if (FAILED(Render_Shadow()))
		return;

	if (FAILED(Render_Deferred()))
		return;

	if (FAILED(Render_LightAcc()))
		return;

	if (FAILED(Render_Blend()))
		return;

	Render_Alpha();
	Render_UI();

#ifdef _DEBUG
	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Deferred");
	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_LightAcc");
	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Shadow");
#endif
}

void CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_Object();
			Safe_Release(pGameObject);
		}
	}
	m_RenderGroup[RENDER_PRIORITY].clear();
}

HRESULT CRenderer::Render_Shadow()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	// 필요한 타겟들을 장치에 셋한다.
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Shadow")))
		return E_FAIL;

	for (auto& pGameObject : m_RenderGroup[RENDER_SHADOW])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_Shadow();
			Safe_Release(pGameObject);
		}
	}
	m_RenderGroup[RENDER_SHADOW].clear();

	// 원상 복구한다
	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Shadow")))
		return E_FAIL;

	return S_OK;
}

void CRenderer::Render_NoneAlpha()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_Object();
			Safe_Release(pGameObject);
		}
	}
	m_RenderGroup[RENDER_NONALPHA].clear();
}

_bool Compare_Z(CGameObject* pDest, CGameObject* pSour)
{
	return pDest->Get_ViewZ() > pSour->Get_ViewZ();
}

void CRenderer::Render_Alpha()
{
	m_RenderGroup[RENDER_ALPHA].sort(Compare_Z);

	for (auto& pGameObject : m_RenderGroup[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_Object();
			Safe_Release(pGameObject);
		}
	}
	m_RenderGroup[RENDER_ALPHA].clear();
}

void CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_Object();
			Safe_Release(pGameObject);
		}
	}
	m_RenderGroup[RENDER_UI].clear();
}

HRESULT CRenderer::Render_Deferred()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	// 필요한 타겟들을 장치에 셋한다.
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Deferred")))
		return E_FAIL;

	Render_NoneAlpha();

	// 원상 복구한다
	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Deferred")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (nullptr == m_pShader_LightAcc)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_LightAcc")))
		return E_FAIL;

	LPD3DXEFFECT pEffect = m_pShader_LightAcc->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	pEffect->AddRef();

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_NormalTexture", L"Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DepthTexture", L"Target_Depth")))
		return E_FAIL;

	_matrix matViewInv, matProjInv;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matViewInv);
	D3DXMatrixInverse(&matViewInv, nullptr, &matViewInv);

	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProjInv);
	D3DXMatrixInverse(&matProjInv, nullptr, &matProjInv);

	pEffect->SetMatrix("g_matViewInv", &matViewInv);
	pEffect->SetMatrix("g_matProjInv", &matProjInv);

	pEffect->SetVector("g_vCamPosition", (_vec4*)&matViewInv.m[3][0]);

	// 셰이더비긴
	pEffect->Begin(nullptr, 0);

	CLightMgr::GetInstance()->Render_Light_Manager(pEffect);

	// 셰이더ㅇ렌드
	pEffect->End();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_LightAcc")))
		return E_FAIL;

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CRenderer::Render_Blend()
{
	// Diffuse * Shade 를 해서 백버퍼에 그리겠다.
	LPD3DXEFFECT pEffect = m_pShader_Blend->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	pEffect->AddRef();

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DiffuseTexture", L"Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_ShadeTexture", L"Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_SpecularTexture", L"Target_Specular")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DepthTexture", L"Target_Depth")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_ShadowTexture", L"Target_Shadow")))
		return E_FAIL;

	// Target_Shadow 추가한다.
	_matrix		matView, matProj;

	_vec3 vEye = { 9.5f, 0.1f, 0.f };
	_vec3 vAt = { 9.5f, 0.05f, 5.f };

	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &_vec3(0.f, 1.f, 0.f));
	D3DXMatrixInverse(&matView, nullptr, &matView);
	D3DXMatrixPerspectiveFovLH(&matProj, 90.f, _float(800.f) / 600.f, 0.1f, 500.f);

	D3DXMatrixInverse(&matProj, nullptr, &matProj);

	pEffect->SetMatrix("g_matViewInv", &matView);
	pEffect->SetMatrix("g_matProjInv", &matProj);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pGraphicDev->SetFVF(FVF_SCREEN);
	m_pGraphicDev->SetIndices(m_pIB);
	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);


	return NOERROR;
}

CRenderer * CRenderer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRenderer*	pInstance = new CRenderer(pGraphicDev);

	if (FAILED(pInstance->Ready_Component()))
	{
		ERR_BOX("Failed to Create CRenderer Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone()
{
	AddRef();

	m_pTarget_Manager->AddRef();
	return this;
}

void Engine::CRenderer::Free(void)
{
	Safe_Release(m_pShader_LightAcc);
	Safe_Release(m_pShader_Blend);
	Safe_Release(m_pTarget_Manager);

	for (size_t i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderGroup[i])
		{
			Safe_Release(pGameObject);
		}
		m_RenderGroup[i].clear();
	}

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	Safe_Release(m_pGraphicDev);
}
