#include "Light.h"

USING(Engine)

CLight::CLight(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

CLight::~CLight()
{
}

HRESULT CLight::Ready_Light(const D3DLIGHT9 & LightInfo)
{
	m_LightInfo = LightInfo;

	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(sizeof(VTXSCREEN) * 4, 0, FVF_SCREEN, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	D3DVIEWPORT9		ViewPort;
	m_pGraphic_Device->GetViewport(&ViewPort);

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

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(sizeof(INDEX16) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
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



	return NOERROR;
}

HRESULT CLight::Render_Light(LPD3DXEFFECT pEffect)
{
	_uint		iPassIdx = 0;

	if (m_LightInfo.Type == D3DLIGHT_DIRECTIONAL)
	{
		iPassIdx = 0;
		pEffect->SetVector("g_vLightDir", &_vec4(m_LightInfo.Direction, 0.f));
	}
	else if (m_LightInfo.Type == D3DLIGHT_POINT)
	{
		iPassIdx = 1;
		pEffect->SetVector("g_vLightPos", &_vec4(m_LightInfo.Position, 1.f));
		pEffect->SetFloat("g_fRange", m_LightInfo.Range);
	}

	pEffect->SetVector("g_vLightDiffuse", (_vec4*)&m_LightInfo.Diffuse);
	pEffect->SetVector("g_vLightAmbient", (_vec4*)&m_LightInfo.Ambient);
	pEffect->SetVector("g_vLightSpecular", (_vec4*)&m_LightInfo.Specular);

	// 셰이더를 비긴패스
	pEffect->BeginPass(iPassIdx);

	pEffect->CommitChanges();

	// 사각형버퍼 렌더링(왜? : 쉐이드 타겟의 모든 픽셀을 채워주기위해서)
	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pGraphic_Device->SetFVF(FVF_SCREEN);
	m_pGraphic_Device->SetIndices(m_pIB);
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	// 엔드패스
	pEffect->EndPass();

	return NOERROR;
}

CLight * CLight::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9 & LightInfo)
{
	CLight*	pInstance = new CLight(pGraphic_Device);

	if (FAILED(pInstance->Ready_Light(LightInfo)))
	{
		ERR_BOX("Failed to Create CLight Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
	Safe_Release(m_pGraphic_Device);
}
