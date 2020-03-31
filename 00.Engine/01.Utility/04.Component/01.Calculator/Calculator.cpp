#include "Calculator.h"
#include "TerrainTex.h"

USING(Engine)

CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CCalculator::~CCalculator(void)
{
}

HRESULT CCalculator::Ready_Calculator(void)
{
	return S_OK;
}

_float CCalculator::Compute_HeightOnTerrain(const _vec3 * pPos, const _vec3 * pTerrainVtxPos, const _ulong & dwCntX, const _ulong & dwCntZ)
{
	_ulong dwIndex = _ulong(pPos->z / 1.f) * dwCntX + _ulong(pPos->x / 1.f);

	_float fRatioX = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / 1.f;
	_float fRatioZ = (pTerrainVtxPos[dwIndex + dwCntZ].z - pPos->z) / 1.f;

	_float fHeight[4] = {   pTerrainVtxPos[dwIndex + dwCntX].y,
							pTerrainVtxPos[dwIndex + dwCntX + 1].y,
							pTerrainVtxPos[dwIndex + 1].y,
							pTerrainVtxPos[dwIndex].y };

	if (fRatioX > fRatioZ)
	{
		return fHeight[0] + (fHeight[1] - fHeight[0]) * fRatioX + (fHeight[2] - fHeight[1]) * fRatioZ;
	}
	else
	{
		return fHeight[0] + (fHeight[2] - fHeight[3]) * fRatioX + (fHeight[3] - fHeight[0]) * fRatioZ;
	}
}

bool CCalculator::Picking_OnTerrain(_vec3* pOut, HWND hWnd, const CTerrainTex * pTerrainTexBufferCom, const CTransform * pTerrainTransCom)
{
	m_mapTile.clear(); // 먼저 비워줌.

	POINT ptMouse{};


	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3 vMousePos;
	D3DVIEWPORT9 ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);
	//뷰포트 -> 투영
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	//투영 -> 뷰스페이스
	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	//뷰스페이스 -> 월드
	_vec3 vRayDir, vRayPos;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);


	_ulong dwVtxCntX = pTerrainTexBufferCom->Get_VtxCntX();
	_ulong dwVtxCntZ = pTerrainTexBufferCom->Get_VtxCntZ();

	const _vec3* pTerrainVtx = pTerrainTexBufferCom->Get_VtxPos();

	_ulong dwVtxIdx[3];
	_float fU, fV, fDist;

	for (_ulong i = 0; i < dwVtxCntX - 1; ++i)
	{
		for (_ulong j = 0; j < dwVtxCntZ - 1; ++j)
		{
			_ulong dwIndex = i * dwVtxCntX + j;

			dwVtxIdx[0] = dwIndex + dwVtxCntX;
			dwVtxIdx[1] = dwIndex + dwVtxCntX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (D3DXIntersectTri(&pTerrainVtx[dwVtxIdx[0]],
				&pTerrainVtx[dwVtxIdx[1]],
				&pTerrainVtx[dwVtxIdx[2]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				_vec3 vOut = pTerrainVtx[dwVtxIdx[0]] +
					fU * (pTerrainVtx[dwVtxIdx[1]] - pTerrainVtx[dwVtxIdx[0]])
					+ fV * (pTerrainVtx[dwVtxIdx[2]] - pTerrainVtx[dwVtxIdx[0]]);
				/*return _vec3(pTerrainVtx[dwVtxIdx[1]].x + 
							(pTerrainVtx[dwVtxIdx[1]].x - pTerrainVtx[dwVtxIdx[0]].x) * fU,
							0.f,
							pTerrainVtx[dwVtxIdx[1]].z + 
							(pTerrainVtx[dwVtxIdx[1]].z - pTerrainVtx[dwVtxIdx[2]].z) * fV);*/

				m_mapTile.emplace(fDist, vOut);
			}

			dwVtxIdx[0] = dwIndex + dwVtxCntX;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (D3DXIntersectTri(&pTerrainVtx[dwVtxIdx[0]],
				&pTerrainVtx[dwVtxIdx[1]],
				&pTerrainVtx[dwVtxIdx[2]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				_vec3 vOut = pTerrainVtx[dwVtxIdx[0]] +
					fU * (pTerrainVtx[dwVtxIdx[1]] - pTerrainVtx[dwVtxIdx[0]])
					+ fV * (pTerrainVtx[dwVtxIdx[2]] - pTerrainVtx[dwVtxIdx[0]]);

				/*return _vec3(pTerrainVtx[dwVtxIdx[2]].x +
					(pTerrainVtx[dwVtxIdx[1]].x - pTerrainVtx[dwVtxIdx[2]].x) * fU,
					0.f,
					pTerrainVtx[dwVtxIdx[2]].z +
					(pTerrainVtx[dwVtxIdx[0]].z - pTerrainVtx[dwVtxIdx[2]].z) * fV);*/
				m_mapTile.emplace(fDist, vOut);
			}
		}
	}

	if (m_mapTile.empty())
		return false;
	else
	{
		*pOut = m_mapTile.begin()->second;
		return true;
	}
}

_bool CCalculator::Picking_Sphere(HWND hWnd, _matrix * pMatWorld)
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3 vMousePos;
	D3DVIEWPORT9 ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);
	//뷰포트 -> 투영
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	//투영 -> 뷰스페이스
	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	//뷰스페이스 -> 월드
	_vec3 vRayDir, vRayPos;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);

	// 월드 -> 로컬
	D3DXMATRIX matWorld = *pMatWorld;
	D3DXMatrixInverse(&matWorld, nullptr, &matWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);
	D3DXVec3Normalize(&vRayDir, &vRayDir);

	float b = D3DXVec3Dot(&vRayPos, &vRayDir);
	float a = D3DXVec3Dot(&vRayDir, &vRayDir);
	float c = (vRayPos.x * vRayPos.x
		+ vRayPos.y * vRayPos.y
		+ vRayPos.z * vRayPos.z) - (1.f);
	float fresult = b*b - a * c;

	if (fresult >= 0.f)
		return true;

	return false;
}

_bool CCalculator::Collision_AABB(const _vec3 * pDestMin, const _vec3 * pDestMax, const _matrix * pDestWorld, const _vec3 * pSourMin, const _vec3 * pSourMax, const _matrix * pSourWorld)
{
	_vec3	vDestMin, vDestMax, vSourMin, vSourMax;
	_float	fMin, fMax;

	D3DXVec3TransformCoord(&vDestMin, pDestMin, pDestWorld);
	D3DXVec3TransformCoord(&vDestMax, pDestMax, pDestWorld);

	D3DXVec3TransformCoord(&vSourMin, pSourMin, pSourWorld);
	D3DXVec3TransformCoord(&vSourMax, pSourMax, pSourWorld);

	fMin = max(vDestMin.x, vSourMin.x);
	fMax = min(vDestMax.x, vSourMax.x);

	if (fMin > fMax)
		return false;

	fMin = max(vDestMin.y, vSourMin.y);
	fMax = min(vDestMax.y, vSourMax.y);

	if (fMin > fMax)
		return false;

	fMin = max(vDestMin.z, vSourMin.z);
	fMax = min(vDestMax.z, vSourMax.z);

	if (fMin > fMax)
		return false;

	return true;
}

_bool CCalculator::Collision_OBB(const _vec3 * pDestMin, const _vec3 * pDestMax, const _matrix * pDestWorld, const _vec3 * pSourMin, const _vec3 * pSourMax, const _matrix * pSourWorld)
{
	OBB		tOBB[2];
	ZeroMemory(tOBB, sizeof(OBB) * 2);
	// 박스의 8개 정점과 박스 중점을 저장한다.(로컬)
	Set_Point(&tOBB[0], pDestMin, pDestMax);
	Set_Point(&tOBB[1], pSourMin, pSourMax);
	// 로컬좌표인 좌표들을 월드화 시킨다.
	for (_uint i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&tOBB[0].vPoint[i], &tOBB[0].vPoint[i], pDestWorld);
		D3DXVec3TransformCoord(&tOBB[1].vPoint[i], &tOBB[1].vPoint[i], pSourWorld);
	}

	D3DXVec3TransformCoord(&tOBB[0].vCenter, &tOBB[0].vCenter, pDestWorld);
	D3DXVec3TransformCoord(&tOBB[1].vCenter, &tOBB[1].vCenter, pSourWorld);

	for (_uint i = 0; i < 2; ++i)
		Set_Axis(&tOBB[i]); // 중점에서 각 면으로 뻗어나가는 벡터와 축들을 저장.

	_float fDistance[3];
	// 1. 첫 번째 obb에서 중점에서 뻗어나가는 벡터를 임의의 축으로 투영한 길이의 합
	// 2. 두 번째 obb에서 중점에서 뻗어나가는 벡터를 임의의 축으로 투영한 길이의 합
	// 3. 중점에서 중점으로 이은 벡터를 임의의 축으로 투영한 길이.

	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			fDistance[0] = fabs(D3DXVec3Dot(&tOBB[0].vProjAxis[0], &tOBB[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tOBB[0].vProjAxis[1], &tOBB[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tOBB[0].vProjAxis[2], &tOBB[i].vAxis[j]));

			fDistance[1] = fabs(D3DXVec3Dot(&tOBB[1].vProjAxis[0], &tOBB[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tOBB[1].vProjAxis[1], &tOBB[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tOBB[1].vProjAxis[2], &tOBB[i].vAxis[j]));

			fDistance[2] = fabs(D3DXVec3Dot(&(tOBB[1].vCenter - tOBB[0].vCenter), &tOBB[i].vAxis[j]));

			if (fDistance[2] > fDistance[0] + fDistance[1])
				return false;
		}
	}

	return true;
}

void CCalculator::Set_Point(OBB * pObb, const _vec3 * pMin, const _vec3 * pMax)
{
	pObb->vPoint[0] = _vec3(pMin->x, pMax->y, pMin->z);
	pObb->vPoint[1] = _vec3(pMax->x, pMax->y, pMin->z);
	pObb->vPoint[2] = _vec3(pMax->x, pMin->y, pMin->z);
	pObb->vPoint[3] = _vec3(pMin->x, pMin->y, pMin->z);

	pObb->vPoint[4] = _vec3(pMin->x, pMax->y, pMax->z);
	pObb->vPoint[5] = _vec3(pMax->x, pMax->y, pMax->z);
	pObb->vPoint[6] = _vec3(pMax->x, pMin->y, pMax->z);
	pObb->vPoint[7] = _vec3(pMin->x, pMin->y, pMax->z);

	pObb->vCenter = (*pMax + *pMin) * 0.5f;
}

void CCalculator::Set_Axis(OBB * pObb)
{
	pObb->vProjAxis[0] = (pObb->vPoint[2] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[1] = (pObb->vPoint[0] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[2] = (pObb->vPoint[7] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;

	pObb->vAxis[0] = pObb->vPoint[2] - pObb->vPoint[3];
	pObb->vAxis[1] = pObb->vPoint[0] - pObb->vPoint[3];
	pObb->vAxis[2] = pObb->vPoint[7] - pObb->vPoint[3];

	for (_uint i = 0; i < 3; ++i)
		D3DXVec3Normalize(&pObb->vAxis[i], &pObb->vAxis[i]);
}

CCalculator * CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCalculator* pInstance = new CCalculator(pGraphicDev);

	if (FAILED(pInstance->Ready_Calculator()))
	{
		ERR_BOX("Calculator Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCalculator::Free(void)
{
	m_mapTile.clear();
	Safe_Release(m_pGraphicDev);
}
