#include "Mesh.h"

USING(Engine)

Engine::CMesh::CMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CResources(pGraphicDev)
	, m_bClone(false)
{

}

Engine::CMesh::CMesh(const CMesh& rhs)
	: CResources(rhs)
	, m_bClone(true)
{

}

Engine::CMesh::~CMesh(void)
{

}

void Engine::CMesh::Free(void)
{
	CResources::Free();
}

