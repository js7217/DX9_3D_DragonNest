#include "TimeMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTimeMgr)


Engine::CTimeMgr::CTimeMgr(void)
{

}

Engine::CTimeMgr::~CTimeMgr(void)
{
	Free();
}

_float Engine::CTimeMgr::Get_TimeDelta(const _tchar* pTimerTag)
{
	CTimer*		pTimer = Find_Timer(pTimerTag);

	if (nullptr == pTimer)
		return 0.f;


	return pTimer->Get_TimeDelta();
}

void Engine::CTimeMgr::SetUp_TimeDelta(const _tchar* pTimerTag)
{
	CTimer*		pTimer = Find_Timer(pTimerTag);

	if (nullptr == pTimer)
		return;

	pTimer->SetUp_TimeDelta();
}

HRESULT Engine::CTimeMgr::Ready_Timer(const _tchar* pTimerTag)
{
	CTimer*		pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	NULL_CHECK_RETURN(pTimer, E_FAIL);

	m_mapTimer.emplace(pTimerTag, pTimer);

	return S_OK;
}

Engine::CTimer* Engine::CTimeMgr::Find_Timer(const _tchar* pTimerTag)
{
	auto	iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}

void Engine::CTimeMgr::Free(void)
{
	for_each(m_mapTimer.begin(), m_mapTimer.end(), CDeleteMap());
	m_mapTimer.clear();
}