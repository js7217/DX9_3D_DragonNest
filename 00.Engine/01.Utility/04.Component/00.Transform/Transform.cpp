#include "Transform.h"

USING(Engine)

Engine::CTransform::CTransform(void)
	: m_vScale(1.f, 1.f, 1.f)
	, m_vAngle(0.f, 0.f, 0.f)
{
}

Engine::CTransform::~CTransform(void)
{

}

void Engine::CTransform::Get_Info(INFO eType, _vec3* pInfo)
{
	memcpy(pInfo, &m_matWorld.m[eType][0], sizeof(_vec3));
}

const Engine::_vec3* Engine::CTransform::Get_Info(INFO eType)
{
	memcpy(&m_vInfo[eType], &m_matWorld.m[eType][0], sizeof(_vec3));

	return &m_vInfo[eType];
}

void Engine::CTransform::Set_Pos(const _vec3* const pDir)
{
	m_vInfo[INFO_POS] += *pDir;
}

void CTransform::Set_Pos(const _float & fX, const _float & fY, const _float & fZ)
{
	m_vInfo[INFO_POS] = _vec3{ fX, fY, fZ };
}

void CTransform::Set_Scale(const _float & fX, const _float & fY, const _float & fZ)
{
	m_vScale.x = fX;
	m_vScale.y = fY;
	m_vScale.z = fZ;
}

void Engine::CTransform::Rotation(ROTATION eType, const _float& fAngle)
{
	*(((_float*)&m_vAngle) + eType) += fAngle;
}

void CTransform::Get_WorldMatrix(_matrix * pWorldMatrix) const
{
	*pWorldMatrix = m_matWorld;
}

void CTransform::Move_TargetPos(const _vec3 * pTargetPos, const _float & fSpeed, const _float & fTimeDelta)
{
	_vec3 vDir = *pTargetPos - m_vInfo[INFO_POS];
	m_vInfo[INFO_POS] += *D3DXVec3Normalize(&vDir, &vDir) * fSpeed * fTimeDelta;
}

void CTransform::Set_ParentMatrix(const _matrix & matParent)
{
	m_matWorld *= matParent;
}

void Engine::CTransform::Chase_Target(const _vec3* pTargetPos, const _float& fSpeed)
{
	_vec3 vDir = *pTargetPos - m_vInfo[INFO_POS];

	m_vInfo[INFO_POS] += *D3DXVec3Normalize(&vDir, &vDir) * fSpeed;

	_matrix matRot = *Compute_LookAtTarget(pTargetPos);
	_matrix matScale, matTrans;

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&matTrans, m_vInfo[INFO_POS].x, m_vInfo[INFO_POS].y, m_vInfo[INFO_POS].z);

	m_matWorld = matScale * matRot * matTrans;
}

const Engine::_matrix* Engine::CTransform::Compute_LookAtTarget(const _vec3* pTargetPos)
{
	_vec3	vDir = *pTargetPos - m_vInfo[INFO_POS];
	_vec3	vAxis = *D3DXVec3Cross(&vAxis, &m_vInfo[INFO_UP], &vDir);
	// 임의의 축을 외적을 통해 구한다.
	_vec3	vUp;
	_matrix matRot;

	return D3DXMatrixRotationAxis(&matRot, &vAxis,
		acosf(D3DXVec3Dot(D3DXVec3Normalize(&vDir, &vDir),
			D3DXVec3Normalize(&vUp, &m_vInfo[INFO_UP]))));
}

void CTransform::Chase_Target_RotY(const _vec3 & vTargetPos, ROTATION eRot)		// 회전값을 구해서 Y축 회전에 사용할 것이다.
{
	_vec3 vDir = vTargetPos - m_vInfo[INFO_POS];
	D3DXVec3Normalize(&vDir, &vDir);
	D3DXVec3Normalize(&m_vLook, &m_vLook);

	_float fAngle = acosf(D3DXVec3Dot(&vDir, &m_vLook));
	switch (eRot)
	{
	case Engine::ROT_X:
		if (m_vInfo[INFO_POS].z <= vTargetPos.z)
			fAngle *= -1.f;
		break;
	case Engine::ROT_Y:
		break;
	case Engine::ROT_Z:
		if (m_vInfo[INFO_POS].x >= vTargetPos.x)
			fAngle *= -1.f;
		break;
	default:
		break;
	}

	m_vAngle[ROT_Y] = fAngle;
}
	
const _vec3 & CTransform::Get_Dir(ROTATION eRot)				//  현재 matWorld에서의 방향 벡터를 연산하고 반환해준다.
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	memcpy(&m_vDir, m_matWorld.m[eRot], sizeof(_vec3));
	D3DXVec3Normalize(&m_vDir, &m_vDir);

	return m_vDir;
}

const _matrix * CTransform::Get_NRotWorldMatrix(_matrix * pWorldMatrix) const
{
	if (nullptr != pWorldMatrix)
		*pWorldMatrix = m_matNRotWorld;

	return &m_matNRotWorld;
}

HRESULT Engine::CTransform::Ready_Transform(const _vec3& vLook)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matNRotWorld);

	for (_ulong i = 0; i < 4; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));

	m_vDir = vLook;

	m_vLook = vLook;

	return S_OK;
}

_int Engine::CTransform::Update_Component(const _float& fTimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);

	for (_ulong i = 0; i < INFO_POS; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));

	//  크기 변환
	for (_ulong i = 0; i < INFO_POS; ++i)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
		m_vInfo[i] *= *(((_float*)&m_vScale) + i);
	}

	for (_ulong i = 0; i < INFO_END; ++i)
		memcpy(&m_matNRotWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));

	// 회전 변환
	_matrix matRot[ROT_END];

	D3DXMatrixRotationX(&matRot[ROT_X], m_vAngle.x);
	D3DXMatrixRotationY(&matRot[ROT_Y], m_vAngle.y);
	D3DXMatrixRotationZ(&matRot[ROT_Z], m_vAngle.z);

	for (_ulong i = 0; i < INFO_POS; ++i)
	{
		for (_ulong j = 0; j < ROT_END; ++j)
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
	}

	// 이동 변환
	for (_ulong i = 0; i < INFO_END; ++i)
		memcpy(&m_matWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));

	return 0;
}

Engine::CTransform* Engine::CTransform::Create(const _vec3& vLook)
{
	CTransform* pInstance = new CTransform;

	if (FAILED(pInstance->Ready_Transform(vLook)))
	{
		ERR_BOX("CTransform Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CTransform::Free(void)
{

}
