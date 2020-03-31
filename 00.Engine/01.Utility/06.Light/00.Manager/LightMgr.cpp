#include "LightMgr.h"

USING(Engine)

IMPLEMENT_SINGLETON(CLightMgr)

CLightMgr::CLightMgr(void)
	: m_iIndex(0)
{
}

CLightMgr::~CLightMgr(void)
{
	Free();
}

HRESULT CLightMgr::Add_Light(LPDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9 & LightInfo, const _uint& iIndex)
{
	CLight*		pLight = CLight::Create(pGraphic_Device, LightInfo);
	if (nullptr == pLight)
		return E_FAIL;

	m_mapLight[iIndex].emplace_back(pLight);

	return NOERROR;
}

HRESULT CLightMgr::Render_Light_Manager(LPD3DXEFFECT pEffect)
{
	for (auto& pLight : m_mapLight[100])
	{
		if (nullptr != pLight)
			pLight->Render_Light(pEffect);
	}

	for (auto& pLight : m_mapLight[m_iIndex])
	{
		if (nullptr != pLight)
			pLight->Render_Light(pEffect);
	}
	//for (auto& pLight : m_LightList)
	//{
	//	if (nullptr != pLight)
	//		pLight->Render_Light(pEffect);
	//}

	return NOERROR;
}

void CLightMgr::Free(void)
{
	for (auto& Pair : m_mapLight)
	{
		for(auto& pLight : Pair.second)
			Safe_Release(pLight);

		Pair.second.clear();
	}
	m_mapLight.clear();
	//for (auto& pLight : m_LightList)
	//	Safe_Release(pLight);

	//m_LightList.clear();
}
