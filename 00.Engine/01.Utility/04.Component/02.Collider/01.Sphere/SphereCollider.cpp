#include "SphereCollider.h"


USING(Engine)

CSphereCollider::CSphereCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CSphereCollider::~CSphereCollider(void)
{
}

HRESULT CSphereCollider::Ready_Collider(const LPD3DXMESH& pMesh)
{
	BYTE*		pV;

	pMesh->LockVertexBuffer(0, (void**)&pV);

	if (FAILED(D3DXComputeBoundingSphere((_vec3*)pV, pMesh->GetNumVertices(),
		pMesh->GetNumBytesPerVertex(), &m_vCenter, &m_fRadius)))
	{
		ERR_BOX("Sphere Mesh Bounding Failed");
		return E_FAIL;
	}

	pMesh->UnlockVertexBuffer();

	if (FAILED(D3DXCreateSphere(m_pGraphicDev, m_fRadius, 10, 10, &m_pSphere, NULL)))
	{
		ERR_BOX("Sphere Mesh Create Failed");
		return E_FAIL;
	}


	_matrix matTrans;
	D3DXMatrixTranslation(&matTrans, m_vCenter.x, m_vCenter.y, m_vCenter.z);

	m_matColWorld *= matTrans;
	return S_OK;
}

void CSphereCollider::Render_Collider(const _matrix* pColliderMatrix)
{
	_matrix matTrans;
	D3DXMatrixTranslation(&matTrans, m_vCenter.x, m_vCenter.y, m_vCenter.z);

	m_matColWorld = matTrans * (*pColliderMatrix);
	// 현재 객체의 월드를 구하고 해당 부모를 곱해주는 형태로 가야함.

	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));

	mtrl.Diffuse.r = mtrl.Ambient.r = 0.f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.f;

	m_pGraphicDev->SetMaterial(&mtrl);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matColWorld);
	m_pGraphicDev->SetTexture(0, nullptr);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pSphere->DrawSubset(0);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

CSphereCollider * CSphereCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, const LPD3DXMESH& pMesh)
{
	CSphereCollider* pInstance = new CSphereCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Collider(pMesh)))
		Safe_Release(pInstance);

	return pInstance;
}

void CSphereCollider::Free()
{
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pSphere);
}
