#include "Target.h"

USING(Engine)

CTarget::CTarget(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

HRESULT CTarget::Ready_Target(_uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor)
{
	// Format�� ��� �븻���Ͱ��� �Ϲ����� ������ ������� �ʰ� �Ǽ������� �޾ƾ��ϴ� ��찡 ����� ������ ������ ó��.
	// D3DPOOL�� ���̴� ���� ��� ��ġ���� �Ͼ�� ������ ���� ���� �ϵ���� ������ �����ϴ°��� �� ���⿡ ���ϴ�.

	if (FAILED(D3DXCreateTexture(m_pGraphic_Device, Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, &m_pTarget_Texture)))
		return E_FAIL;

	// ������ �ؽ��ĸ� ����� �ش� �ؽ��Ŀ��� GetSurfaceLevel �Լ��� �̿��ؼ�
	// Surface�� ������ش�. �� Surface�� ����Ÿ���� �����Ҷ� ����Ѵ�.

	if (FAILED(m_pTarget_Texture->GetSurfaceLevel(0, &m_pTarget_Surface)))
		return E_FAIL;

	// ����Ÿ�ٸ��� �ʱ�ȭ �ؾ��ϴ� ������ �ٸ���.
	m_ClearColor = ClearColor;

	return NOERROR;
}

HRESULT CTarget::Clear_Target()
{
	// �׷��ֱ� �� API ��ó�� �ʱ�ȭ �����־�� �Ѵ�. ������ �� ����Ÿ�ٸ��� �ʱ�ȭ �ؾ��ϴ� ������ �ٸ��� ������
	// ������ ��������� �޴´�.
	// �ʱ�ȭ�� ���� ����Ÿ���� ��ġ�� 0��° Ÿ������ �÷����� ��ġ�� �̿��� ����������� ����Ÿ�ٵ��� �ʱ�ȭ�����ش�.

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

	// Old : ���� �����ߴ� ����Ÿ���� �����صд�. (��������� �����ֱ� ����)
	// ����Ÿ���� ���ٸ� nullptr�� �޾ƿ;� �ϱ� ������ NULLCHECK�� ���� �ʴ´�.

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

	//  GetRenderTarget ���� ���۷��� ī��Ʈ�� �ø��� ������ �� �� ����ϸ� ī��Ʈ�� ��������.
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
	// ���� ���۷��� ī��Ʈ�� ���������� GetSurfaceLevel �Լ����� ���۷��� ī��Ʈ�� �ø��� ������
	// �ι� �������ִ°� �´� �����̴�.
}
