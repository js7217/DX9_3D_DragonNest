#include "Shader.h"

USING(Engine)

CShader::CShader(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pEffect(nullptr)
{
	m_pGraphicDev->AddRef();
}

CShader::CShader(const CShader & rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
	, m_pEffect(rhs.m_pEffect)
{
	m_pGraphicDev->AddRef();
	m_pEffect->AddRef();
}

CShader::~CShader(void)
{
}

HRESULT CShader::Ready_Shader(const _tchar * pFilePath)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphicDev,
		pFilePath, NULL, NULL, D3DXSHADER_DEBUG, NULL,
		&m_pEffect, &m_pErrMsg)))
	{
		MessageBoxA(NULL, (char*)m_pErrMsg->GetBufferPointer(), "Shader_Error", MB_OK);
		return E_FAIL;
	}
	else if (nullptr != m_pErrMsg)
	{
		MessageBoxA(NULL, (char*)m_pErrMsg->GetBufferPointer(), "Shader_Warning", MB_OK);
	}

	return S_OK;
}

CShader * CShader::Create(LPDIRECT3DDEVICE9 pGarphicDev, const _tchar * pFilePath)
{
	CShader* pInstance = new CShader(pGarphicDev);

	if (FAILED(pInstance->Ready_Shader(pFilePath)))
	{
		ERR_BOX("Shader Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CShader::Clone(void)
{
	return new CShader(*this);
}

void CShader::Free(void)
{
	Safe_Release(m_pErrMsg);
	Safe_Release(m_pEffect);
	Safe_Release(m_pGraphicDev);
}
