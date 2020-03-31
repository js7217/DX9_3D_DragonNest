#include "Timer.h"

USING(Engine)

Engine::CTimer::CTimer(void)
{

}

Engine::CTimer::~CTimer(void)
{

}

void Engine::CTimer::SetUp_TimeDelta(void)
{
	QueryPerformanceCounter(&m_FrameTime);

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_FixTime = m_FrameTime;
	}

	m_fTimeDelta = (_float(m_FrameTime.QuadPart) - _float(m_LastTime.QuadPart)) / _float(m_CpuTick.QuadPart);

	m_LastTime = m_FrameTime;
}

HRESULT Engine::CTimer::Ready_Timer(void)
{
	QueryPerformanceCounter(&m_FrameTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceCounter(&m_LastTime);
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

Engine::CTimer* Engine::CTimer::Create(void)
{
	CTimer*	pInstance = new CTimer;

	if (FAILED(pInstance->Ready_Timer()))
	{
		ERR_BOX("CTimer Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CTimer::Free(void)
{

}

