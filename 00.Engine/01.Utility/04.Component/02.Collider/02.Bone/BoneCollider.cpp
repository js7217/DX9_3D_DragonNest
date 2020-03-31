#include "BoneCollider.h"


USING(Engine)

CBoneCollider::CBoneCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_fRadius(10.f)
{
	m_pGraphicDev->AddRef();
}

CBoneCollider::~CBoneCollider(void)
{
}

void CBoneCollider::Set_BoneName(const char * pBoneName, Engine::CDynamicMesh * pMeshCom, _matrix* pmatTargetWorld)
{
	if (nullptr == pmatTargetWorld ||
		nullptr == pBoneName ||
		nullptr == pMeshCom)
	{
		ERR_BOX("CBoneCollider : Set_BoneName Func Failed!");
		return;
	}

	const Engine::D3DXFRAME_DERIVED*	pFrame = pMeshCom->Get_FrameByName(pBoneName);
	const _matrix* pMat = &(pFrame->CombinedTransformMatrix);
	m_mapFrame.emplace(pBoneName, pMat);			// 키 : 뼈이름 , 값 : 해당 뼈의 행렬

	m_pmatTargetWorld = pmatTargetWorld;				// 객체의 월드행렬.
}

void CBoneCollider::Set_Radius(const _vec3 & vScale)
{
	m_fRadius = 15.f * 0.01f;		// 임시로 잡은 반지름값. 언젠간 수정이 필요할수도 있음.
	D3DXCreateSphere(m_pGraphicDev, 15.f, 10, 10, &m_pSphere, NULL); // 반지름은 10(로컬)

}

HRESULT CBoneCollider::Ready_Collider()
{
	ZeroMemory(&m_vPos, sizeof(_vec3));

	ZeroMemory(&m_mtrl, sizeof(D3DMATERIAL9));

	m_mtrl.Diffuse.r = m_mtrl.Ambient.r = 1.f;
	m_mtrl.Diffuse.g = m_mtrl.Ambient.g = 1.f;
	m_mtrl.Diffuse.b = m_mtrl.Ambient.b = 1.f;
	m_mtrl.Diffuse.a = m_mtrl.Ambient.a = 1.f;

	return S_OK;
}

_matrix CBoneCollider::Current_BoneMatrix(const char * pBoneName)
{	
	auto iter = m_mapFrame.find(pBoneName);				// 해당 뼈를 찾아서 그 뼈의 애니메이션 상태를 업데이트 시켜준다.

	if (iter == m_mapFrame.end())
	{
		ERR_BOX("pFrame Find Failed");

		_matrix matTemp;
		D3DXMatrixIdentity(&matTemp);
		return matTemp;			//  항등행렬 보냄.
	}

	return *(iter->second) * (*m_pmatTargetWorld);
}

void CBoneCollider::Render_Collider()
{
//	m_pGraphicDev->SetMaterial(&m_mtrl);

	m_pGraphicDev->SetTexture(0, nullptr);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	for (auto& Pair : m_mapFrame)
	{
		_matrix matBone = *(Pair.second) * (*m_pmatTargetWorld);

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matBone);
		m_pSphere->DrawSubset(0);
	}

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

CBoneCollider * CBoneCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBoneCollider* pInstance = new CBoneCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Collider()))
		Safe_Release(pInstance);

	return pInstance;
}

void CBoneCollider::Free()
{
	Safe_Release(m_pSphere);
	Safe_Release(m_pGraphicDev);

	m_mapFrame.clear();
}
