#include "Resources.h"

USING(Engine)

Engine::CResources::CResources(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CResources::CResources(const CResources& rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CResources::~CResources(void)
{

}

void Engine::CResources::Free(void)
{
	Safe_Release(m_pGraphicDev);
}

