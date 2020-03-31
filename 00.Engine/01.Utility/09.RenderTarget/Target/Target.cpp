#include "Target.h"

USING(Engine)

CTarget::CTarget(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

HRESULT CTarget::Ready_Target(_uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor)
{
	// Format의 경우 노말벡터같이 일반적인 색상을 사용하지 않고 실수형으로 받아야하는 경우가 생기기 때문에 변수로 처리.
	// D3DPOOL은 셰이더 연산 경우 장치에서 일어나기 때문에 저장 또한 하드웨어 공간에 저장하는것이 더 쓰기에 편하다.

	if (FAILED(D3DXCreateTexture(m_pGraphic_Device, Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, &m_pTarget_Texture)))
		return E_FAIL;

	// 임의의 텍스쳐를 만들고 해당 텍스쳐에서 GetSurfaceLevel 함수를 이용해서
	// Surface를 만들어준다. 이 Surface는 렌더타겟을 세팅할때 사용한다.

	if (FAILED(m_pTarget_Texture->GetSurfaceLevel(0, &m_pTarget_Surface)))
		return E_FAIL;

	// 렌더타겟마다 초기화 해야하는 색상이 다르다.
	m_ClearColor = ClearColor;

	return NOERROR;
}

HRESULT CTarget::Clear_Target()
{
	// 그려주기 전 API 때처럼 초기화 시켜주어야 한다. 하지만 각 렌더타겟마다 초기화 해야하는 색상이 다르기 때문에
	// 색상을 멤버변수로 받는다.
	// 초기화시 각각 렌더타겟을 장치의 0번째 타겟으로 올려놓고 장치를 이용해 멤버색상으로 렌더타겟들을 초기화시켜준다.

	SetUp_OnGraphicDev(0);

	if (FAILED(m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET, m_ClearColor, 1.f, 0)))
		return E_FAIL;

	Release_OnGraphicDev(0);

	return NOERROR;
}

HRESULT CTarget::SetUp_OnGraphicDev(_ulong dwIndex)
{
	if (nullptr == m_pTarget_Surface ||
		nullptr == m_pGraphic_Device)
		return E_FAIL;

	// Old : 전에 존재했던 렌더타겟을 저장해둔다. (사용했으면 돌려주기 위해)
	// 렌더타겟이 없다면 nullptr을 받아와야 하기 때문에 NULLCHECK를 하지 않는다.

	m_pGraphic_Device->GetRenderTarget(dwIndex, &m_pOld_Surface);

	if (FAILED(m_pGraphic_Device->SetRenderTarget(dwIndex, m_pTarget_Surface)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTarget::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName)
{
	return pEffect->SetTexture(pConstantName, m_pTarget_Texture);
}

HRESULT CTarget::Release_OnGraphicDev(_ulong dwIndex)
{
	if (nullptr == m_pTarget_Surface ||
		nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderTarget(dwIndex, m_pOld_Surface);

	//  GetRenderTarget 또한 레퍼런스 카운트를 늘리기 때문에 꼭 다 사용하면 카운트를 낮춰주자.
	Safe_Release(m_pOld_Surface);

	//if (GetKeyState(VK_RETURN) & 0x8000)
	//{
	//	if (0 == dwIndex)
	//		D3DXSaveTextureToFile(L"../Bin/Diffuse.jpg", D3DXIFF_JPG, m_pTarget_Texture, nullptr);
	//	if (1 == dwIndex)
	//		D3DXSaveTextureToFile(L"../Bin/Normal.jpg", D3DXIFF_JPG, m_pTarget_Texture, nullptr);

	//}

	return NOERROR;
}
#ifdef _DEBUG
HRESULT CTarget::Ready_Debug_Buffer(_float fStartX, _float fStartY, _float fSizeX, _float fSizeY)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(sizeof(VTXSCREEN) * 4, 0, FVF_SCREEN, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXSCREEN*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPos = _vec4(fStartX, fStartY, 0.0f, 1.f);
	pVertices[0].vTexUV = _vec2(0.0f, 0.f);

	pVertices[1].vPos = _vec4(fStartX + fSizeX, fStartY, 0.0f, 1.f);
	pVertices[1].vTexUV = _vec2(1.0f, 0.f);

	pVertices[2].vPos = _vec4(fStartX + fSizeX, fStartY + fSizeY, 0.0f, 1.f);
	pVertices[2].vTexUV = _vec2(1.0f, 1.f);

	pVertices[3].vPos = _vec4(fStartX, fStartY + fSizeY, 0.0f, 1.f);
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

HRESULT CTarget::Render_Debug_Buffer()
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pVB ||
		nullptr == m_pIB)
		return E_FAIL;

	m_pGraphic_Device->SetTexture(0, m_pTarget_Texture);

	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pGraphic_Device->SetFVF(FVF_SCREEN);
	m_pGraphic_Device->SetIndices(m_pIB);
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	return NOERROR;
}
#endif

CTarget * CTarget::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor)
{
	CTarget*	pInstance = new CTarget(pGraphic_Device);

	if (FAILED(pInstance->Ready_Target(Width, Height, Format, ClearColor)))
	{
		ERR_BOX("Failed to Create CTarget Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTarget::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
#endif

	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pTarget_Surface);
	Safe_Release(m_pTarget_Texture);
	// 같은 레퍼런스 카운트를 공유하지만 GetSurfaceLevel 함수에서 레퍼런스 카운트를 늘리기 때문에
	// 두번 삭제해주는게 맞는 선택이다.
}
