#include "Target_Manager.h"
#include "Target.h"

USING(Engine)

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
}

CTarget_Manager::~CTarget_Manager()
{
	Free();
}

HRESULT CTarget_Manager::Add_Target(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pTargetTag, _uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor)
{
	CTarget*	pTarget = Find_Target(pTargetTag);

	if (nullptr != pTarget)
		return E_FAIL;

	pTarget = CTarget::Create(pGraphic_Device, Width, Height, Format, ClearColor);
	if (nullptr == pTarget)
		return E_FAIL;

	m_Targets.insert(TARGETS::value_type(pTargetTag, pTarget));

	return NOERROR;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CTarget*	pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		list<CTarget*>		MRTList;

		MRTList.push_back(pTarget);

		m_MRT.insert(MRT::value_type(pMRTTag, MRTList));
	}
	else
		pMRTList->push_back(pTarget);

	pTarget->AddRef();

	return NOERROR;
}

HRESULT CTarget_Manager::Begin_MRT(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;


	for (auto& pTarget : *pMRTList)
	{
		pTarget->Clear_Target();
	}

	_uint		iIndex = 0;

	for (auto& pTarget : *pMRTList)
	{
		pTarget->SetUp_OnGraphicDev(iIndex++);
	}

	return NOERROR;
}

HRESULT CTarget_Manager::End_MRT(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	_uint		iIndex = 0;

	for (auto& pTarget : *pMRTList)
	{
		pTarget->Release_OnGraphicDev(iIndex++);
	}

	return NOERROR;
}

HRESULT CTarget_Manager::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName, const _tchar * pTargetTag)
{
	CTarget*	pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->SetUp_OnShader(pEffect, pConstantName);
}
#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug_Buffer(const _tchar * pTargetTag, _float fStartX, _float fStartY, _float fSizeX, _float fSizeY)
{
	CTarget*	pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->Ready_Debug_Buffer(fStartX, fStartY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render_Debug_Buffer(const _tchar * pMRTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pTarget : *pMRTList)
	{
		if (nullptr != pTarget)
			pTarget->Render_Debug_Buffer();
	}

	return NOERROR;
}
#endif
CTarget * CTarget_Manager::Find_Target(const _tchar * pTargetTag)
{
	auto	iter = find_if(m_Targets.begin(), m_Targets.end(), CTag_Finder(pTargetTag));

	if (iter == m_Targets.end())
		return nullptr;

	return iter->second;
}

list<CTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_MRT.begin(), m_MRT.end(), CTag_Finder(pMRTTag));

	if (iter == m_MRT.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRT)
	{
		for (auto& pTarget : Pair.second)
			Safe_Release(pTarget);

		Pair.second.clear();
	}
	m_MRT.clear();

	for (auto& Pair : m_Targets)
		Safe_Release(Pair.second);

	m_Targets.clear();
}
