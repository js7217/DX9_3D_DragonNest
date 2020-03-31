#include "NaviMesh.h"

USING(Engine)

CNaviMesh::CNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
	, m_dwCurrentIdx(0)
{
}

CNaviMesh::CNaviMesh(const CNaviMesh & rhs)
	: CMesh(rhs)
	, m_dwCurrentIdx(rhs.m_dwCurrentIdx)
	, m_vecCell(rhs.m_vecCell)
{
	for (auto iter : m_vecCell)
		iter->AddRef();
}

CNaviMesh::~CNaviMesh(void)
{
}

const vector<CCell*>& CNaviMesh::Get_vecCell()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_vecCell;
}

const _int & CNaviMesh::Get_Group(const _int & iIndex)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_vecCell[iIndex]->Get_Group();
}

void CNaviMesh::Set_CellPos(const _vec3 & vPos, const CCell::CELL_OPTION& eOption, const _uint& iGroup)
{
	_vec3 vPickPos = vPos;
	for (auto& pCell : m_vecCell)
	{
		for (_uint i = 0; i < CCell::POINT_END; ++i)
		{
			_vec3 vPointPos = *pCell->Get_Point(CCell::POINT(i));
			_float fDist = D3DXVec3Length(&(vPickPos - vPointPos));

			if (fDist > 0.2f)
				continue;

			vPickPos = vPointPos;
		}
	}

	m_vecPos.emplace_back(vPickPos); // Cell을 만들기 전에 위치값을 보관해두는 곳.

	if (3 == m_vecPos.size()) // 해당 위치값이 3개가 되는 순간 Cell을 만들것이다.
	{
		_vec3 vPos[3];
		for (_int i = 0; i < 3; ++i)
			vPos[i] = m_vecPos[i];

		CCell* pCell = CCell::Create(m_pGraphicDev, m_vecCell.size(),
			&vPos[0], &vPos[1], &vPos[2], eOption, iGroup);
		NULL_CHECK_RETURN(pCell, );
		m_vecCell.emplace_back(pCell);
		m_vecPos.clear();

	}
}

void CNaviMesh::Set_MovePos(const _vec3& vPos)
{
	_matrix matTrans, matScale, matWorld;
	D3DXMatrixScaling(&matScale, 0.1f, 0.1f, 0.1f);

	for (auto& pCell : m_vecCell)
	{
		for (_uint i = 0; i < CCell::POINT_END; ++i)
		{
			_vec3 vPointPos = *pCell->Get_Point(CCell::POINT(i));
			_float fDist = D3DXVec3Length(&(vPos - vPointPos));

			if (fDist > 0.2f)
				continue;

			pCell->Set_Point(CCell::POINT(i), vPos);
		}
	}
}

void CNaviMesh::Set_PointPos(const _vec3 & vPrePos, const _vec3 & vChanagePos)
{
	for (auto& pCell : m_vecCell)
	{
		for (_uint i = 0; i < CCell::POINT_END; ++i)
		{
			_vec3 vPointPos = *pCell->Get_Point(CCell::POINT(i));
			_float fDist = D3DXVec3Length(&(vPrePos - vPointPos));

			if (fDist > 0.2f)
				continue;

			pCell->Set_Point(CCell::POINT(i), vChanagePos);
		}
	}
}

const _float& CNaviMesh::Set_NaviPosY(const _vec3 & vTargetPos)
{
	D3DXPLANE Plane;

	D3DXPlaneFromPoints(&Plane,
		m_vecCell[m_dwCurrentIdx]->Get_Point(CCell::POINT_A),
		m_vecCell[m_dwCurrentIdx]->Get_Point(CCell::POINT_B),
		m_vecCell[m_dwCurrentIdx]->Get_Point(CCell::POINT_C));

	return (-Plane.a * vTargetPos.x
				- Plane.c * vTargetPos.z - Plane.d) / Plane.b;

	//m_vecCell[m_dwCurrentIdx]->Get

	//for (auto&pCell : m_vecCell)
	//{

	//	//if (pCell->Picking_Cell(vTargetPos))				// 먼저 해당 셀을 골라낸다.
	//	//{
	//	//	D3DXPlaneFromPoints(&Plane,
	//	//		pCell->Get_Point(CCell::POINT_A),
	//	//		pCell->Get_Point(CCell::POINT_B),
	//	//		pCell->Get_Point(CCell::POINT_C));

	//	//	return (-Plane.a * vTargetPos.x
	//	//		- Plane.c * vTargetPos.z - Plane.d) / Plane.b; // 이후 Plane을 이용해서  y값을 찾아낸다.
	//	//}
	//}
}

void CNaviMesh::Set_Group(const _int & iIndex, const _uint & iGroup)
{
	m_vecCell[iIndex]->Set_Group(iGroup);
}

_bool CNaviMesh::Delete_Cell(const _vec3 & vPos, _int* pIndex)
{
	auto iter_begin = m_vecCell.begin();
	auto iter_end = m_vecCell.end();
	_int iIndex = 0;
	for (; iter_begin != iter_end; )
	{
		if ((*iter_begin)->Picking_Cell(vPos))
		{
			CCell* pCell = *iter_begin;
			Safe_Release(pCell);
			iter_begin = m_vecCell.erase(iter_begin);
			iter_end = m_vecCell.end();

			*pIndex = iIndex;
			return true;
		}
		else
		{
			++iter_begin;
			iIndex++;
		}
	}
	return false;
}

_bool CNaviMesh::Get_PickCell(const _vec3 & vPos, _int * pIndex)
{
	_int iIndex = 0;
	for (auto& pCell : m_vecCell)
	{
		if (pCell->Picking_Cell(vPos))
		{
			*pIndex = iIndex;
			return true;
		}
		else
			iIndex++;
	}
	return false;
}

_bool CNaviMesh::Delete_Cell(const _int & iIndex)
{
	if (m_vecCell.empty())
		return false;

	auto iter = m_vecCell.begin();

	for (_int i = 0; i < iIndex; ++i)
		++iter;

	CCell* pCell = *iter;

	Safe_Release(pCell);
	iter = m_vecCell.erase(iter);
	
	return true;
}

void CNaviMesh::Clear_Cell(void)
{
	for (auto iter : m_vecCell)
		Safe_Release(iter);

	m_vecCell.clear();
	m_vecCell.shrink_to_fit();
}

void CNaviMesh::Clear_Pos(void)
{
	m_vecPos.clear();
}

void CNaviMesh::Change_CellOption(const _int & iIndex, const CCell::CELL_OPTION & eOption)
{
	m_vecCell[iIndex]->Set_CellOption(eOption);
}

void CNaviMesh::Change_CellOption(const CCell::CELL_OPTION & eDstOption, const CCell::CELL_OPTION & eSrcOption, const _uint & iGroup)
{
	for (auto& pCell : m_vecCell)
	{
		if (eSrcOption == pCell->Get_CellOption() && iGroup == pCell->Get_Group())
			pCell->Set_CellOption(eDstOption);
	}
}

void CNaviMesh::Click_Point(const _vec3 & vPos, _vec3* pOutPos)
{
	_matrix matTrans, matScale, matWorld;
	D3DXMatrixScaling(&matScale, 0.1f, 0.1f, 0.1f);

	for (auto& pCell : m_vecCell)
	{
		pCell->Reset_Check(); // 체크상태 모두 해제.
		for (_uint i = 0; i < CCell::POINT_END; ++i)
		{
			_vec3 vPointPos = *pCell->Get_Point(CCell::POINT(i));
			D3DXMatrixTranslation(&matTrans, vPointPos.x, vPointPos.y, vPointPos.z);
			matWorld = matScale * matTrans;
			
			if (Picking_Sphere(vPos, &matWorld))
			{
				*pOutPos = vPointPos;
				pCell->Set_bIsCheck(CCell::POINT(i)); // 조건에 맞으면 체크표시.
			}
		}
	}
}

_bool CNaviMesh::Picking_Sphere(const _vec3& vMousePos, _matrix * pMatWorld)
{
	_vec3 vPos;
	D3DVIEWPORT9 ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);
	//뷰포트 -> 투영
	vPos.x = vMousePos.x / (ViewPort.Width * 0.5f) - 1.f;
	vPos.y = vMousePos.y / -(ViewPort.Height * 0.5f) + 1.f;
	vPos.z = 0.f;

	//투영 -> 뷰스페이스
	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vPos, &vPos, &matProj);

	//뷰스페이스 -> 월드
	_vec3 vRayDir, vRayPos;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vPos - vRayPos;

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

HRESULT CNaviMesh::Ready_NaviMesh(void)
{
	m_vecPos.reserve(3);
	m_vecCell.reserve(100);

//	CCell* pCell = nullptr;

	//// 0
	//pCell = CCell::Create(m_pGraphicDev, m_vecCell.size(),
	//	&_vec3(0.f, 0.f, 2.f), &_vec3(2.f, 0.f, 0.f), &_vec3(0.f, 0.f, 0.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.emplace_back(pCell);

	//// 1
	//pCell = CCell::Create(m_pGraphicDev, m_vecCell.size(),
	//	&_vec3(0.f, 0.f, 2.f), &_vec3(2.f, 0.f, 2.f), &_vec3(2.f, 0.f, 0.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.emplace_back(pCell);

	//// 2
	//pCell = CCell::Create(m_pGraphicDev, m_vecCell.size(),
	//	&_vec3(0.f, 0.f, 4.f), &_vec3(2.f, 0.f, 2.f), &_vec3(0.f, 0.f, 2.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.emplace_back(pCell);

	//// 3
	//pCell = CCell::Create(m_pGraphicDev, m_vecCell.size(),
	//	&_vec3(2.f, 0.f, 2.f), &_vec3(4.f, 0.f, 0.f), &_vec3(2.f, 0.f, 0.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.emplace_back(pCell);

	//FAILED_CHECK_RETURN(Link_Cell(), E_FAIL);

	return S_OK;
}

void CNaviMesh::Render_NaviMesh(void)
{
	for (auto& iter : m_vecCell)
		iter->Render_Cell();
}

_vec3 CNaviMesh::MoveOn_NaviMesh(const _vec3 * pTargetPos, const _vec3 * pTargetDir, const _float& fTimeDelta, const _float& fSpeed)
{
	_vec3 vEndPos = *pTargetPos + *pTargetDir;
	_vec3 vSliding = {};

	if (CCell::COMPARE_MOVE == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vSliding, pTargetDir))
	{
		if (m_vecCell[m_dwCurrentIdx]->Get_CellOption() == CCell::CELL_OPTION::TRIGGER)
			return *pTargetPos;

		vEndPos.y = Set_NaviPosY(vEndPos);
		return vEndPos;
	}
	else if(CCell::COMPARE_STOP == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vSliding, pTargetDir))
	{
		_vec3 vPos = vSliding * fSpeed * fTimeDelta;	// dir * fSpeed * fTime

		_vec3 vSlidingPos = *pTargetPos + vPos;
		vSlidingPos.y = Set_NaviPosY(vSlidingPos);

		if (CCell::COMPARE_MOVE == m_vecCell[m_dwCurrentIdx]->Compare(&vSlidingPos, &m_dwCurrentIdx, &vSliding, pTargetDir))
		{
			return vSlidingPos;
		}
		else
		{
			return *pTargetPos;
		}
	}

	return _vec3(0.f, 0.f, 0.f);
}


HRESULT CNaviMesh::Link_Cell(void)
{
	for (_ulong i = 0; i < m_vecCell.size(); ++i)
	{
		for (_ulong j = 0; j < m_vecCell.size(); ++j)
		{
			if (i == j)
				continue;

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_AB) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_A),
													m_vecCell[i]->Get_Point(CCell::POINT_B),
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_AB, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_BC) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_B),
													m_vecCell[i]->Get_Point(CCell::POINT_C),
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_BC, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_CA) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_C),
													m_vecCell[i]->Get_Point(CCell::POINT_A),
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_CA, m_vecCell[j]);
				continue;
			}

		}
	}

	return S_OK;
}

CNaviMesh * CNaviMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNaviMesh* pInstance = new CNaviMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_NaviMesh()))
		Safe_Release(pInstance);

	return pInstance;
}

CResources * CNaviMesh::Clone(void)
{
	return new CNaviMesh(*this);
}

void CNaviMesh::Free(void)
{
	CMesh::Free();
	for (auto iter : m_vecCell)
		Safe_Release(iter);

	m_vecCell.clear();
	m_vecCell.shrink_to_fit();
}
