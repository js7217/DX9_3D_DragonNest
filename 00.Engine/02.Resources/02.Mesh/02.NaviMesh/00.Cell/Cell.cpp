#include "Cell.h"

USING(Engine)

CCell::CCell(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CCell::~CCell(void)
{
}

void CCell::Set_Point(POINT eType, const _vec3 & vPos)
{
	if (!m_bClick[eType])
		return;
	m_vPoint[eType] = vPos;

	m_pTriCol->Set_Pos(m_vPoint[POINT_A], m_vPoint[POINT_B], m_vPoint[POINT_C]);
}

void CCell::Set_CellOption(const CELL_OPTION & eOption)
{
	m_eOption = eOption;

	switch (m_eOption)
	{
	case Engine::CCell::NORMAL:
		m_pTriCol->Change_Color(D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
		break;
	case Engine::CCell::TRIGGER:
		m_pTriCol->Change_Color(D3DXCOLOR(0.f, 1.f, 0.f, 1.f));
		break;
	case Engine::CCell::EVENT:
		m_pTriCol->Change_Color(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));
		break;
	default:
		break;
	}
}

void CCell::Set_bIsCheck(POINT eType)
{
	for (_int i = 0; i < POINT_END; ++i)
	{
		if (eType == i)
			m_bClick[i] = true;
		else
			m_bClick[i] = false;
	}
}


_bool CCell::Picking_Cell(const _vec3 & vPos)
{
	for (_ulong i = 0; i < LINE_END; ++i)
	{
		if (CLine::COMPARE_OUT == m_pLine[i]->Compare(&_vec2(vPos.x, vPos.z), &_vec3(0.f, 0.f, 0.f)))
			return false;
	}

	return true; // 모든 라인에서의 안쪽을 가리키게 되면 트루 반환.
}

void CCell::Reset_Check()
{
	for (_int i = 0; i < POINT_END; ++i)
		m_bClick[i] = false;
}

HRESULT CCell::Ready_Cell(const _ulong& dwIndex,
							const _vec3* pPointA,
							const _vec3* pPointB,
							const _vec3* pPointC,
							const CELL_OPTION& eOption, const _uint& iGroup)
{
	m_dwIndex = dwIndex;
	m_iGroup = iGroup;

	m_vPoint[POINT_A] = *pPointA;
	m_vPoint[POINT_B] = *pPointB;
	m_vPoint[POINT_C] = *pPointC; // 정점 세개 만들기.


	m_pLine[LINE_AB] = CLine::Create(&_vec2(m_vPoint[POINT_A].x, m_vPoint[POINT_A].z),
		&_vec2(m_vPoint[POINT_B].x, m_vPoint[POINT_B].z));

	m_pLine[LINE_BC] = CLine::Create(&_vec2(m_vPoint[POINT_B].x, m_vPoint[POINT_B].z),
		&_vec2(m_vPoint[POINT_C].x, m_vPoint[POINT_C].z));

	m_pLine[LINE_CA] = CLine::Create(&_vec2(m_vPoint[POINT_C].x, m_vPoint[POINT_C].z),
		&_vec2(m_vPoint[POINT_A].x, m_vPoint[POINT_A].z));
	// 정점을 가지고 라인 3개 만들기. (방향성 일치 중요)

	D3DXCOLOR dwColor;			//옵션에 따라 컬러 결정해준다.
	switch (eOption)
	{
	case CELL_OPTION::NORMAL:
		dwColor = { 1.f, 0.f, 0.f, 1.f };
		break;
	case CELL_OPTION::TRIGGER:
		dwColor = { 0.f, 1.f, 0.f, 1.f };
		break;
	case CELL_OPTION::EVENT:
		dwColor = { 0.f, 0.f, 1.f, 1.f };
		break;
	default:
		break;
	}

	m_eOption = eOption;

	m_pTriCol = CCellTriCol::Create(m_pGraphicDev, *pPointA, *pPointB, *pPointC, dwColor);
	NULL_CHECK_RETURN(m_pTriCol, E_FAIL);

#ifdef _DEBUG
	FAILED_CHECK_RETURN(D3DXCreateLine(m_pGraphicDev, &m_pD3DXLine), E_FAIL);
#endif // _DEBUG


	ZeroMemory(&m_mtrl, sizeof(D3DMATERIAL9));

	m_mtrl.Diffuse.r = m_mtrl.Ambient.r = 1.f;
	m_mtrl.Diffuse.g = m_mtrl.Ambient.g = 1.f;
	m_mtrl.Diffuse.b = m_mtrl.Ambient.b = 1.f;
	m_mtrl.Diffuse.a = m_mtrl.Ambient.a = 1.f;

	return S_OK;
}

_bool CCell::Compare_Point(const _vec3* pFirstPoint, const _vec3* pSecondPoint, CCell* pCell)
{
	// 이웃하는 셀이 누구인지 만들어주는 함수.
	if (m_vPoint[POINT_A] == *pFirstPoint)
	{
		if (m_vPoint[POINT_B] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_AB] = pCell;
			return true;
		}

		if (m_vPoint[POINT_C] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_CA] = pCell;
			return true;
		}
	}

	if (m_vPoint[POINT_B] == *pFirstPoint)
	{
		if (m_vPoint[POINT_A] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_AB] = pCell;
			return true;
		}

		if (m_vPoint[POINT_C] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_BC] = pCell;
			return true;
		}
	}

	if (m_vPoint[POINT_C] == *pFirstPoint)
	{
		if (m_vPoint[POINT_A] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_CA] = pCell;
			return true;
		}

		if (m_vPoint[POINT_B] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_BC] = pCell;
			return true;
		}
	}

	return false;
}

CCell::COMPARE CCell::Compare(_vec3 * pEndPos, _ulong * pCellIndex, _vec3* pSliding, const _vec3* pTargetDir)
{
	for (_ulong i = 0; i < LINE_END; ++i)
	{
		if (CLine::COMPARE_OUT == m_pLine[i]->Compare(&_vec2(pEndPos->x, pEndPos->z), pTargetDir))
		{
			if (nullptr == m_pNeighbor[i]) // 이웃한 Cell 이 없다면 멈춘다 -> 슬라이딩 벡터를 탄다.
			{
				_vec2 vSliding = m_pLine[i]->Make_SlidingVector(&_vec2(pEndPos->x, pEndPos->z), pTargetDir);

				*pSliding = _vec3(vSliding.x, 0.f, vSliding.y);
				*pCellIndex = m_dwIndex;
				// *pSliding = m_pLine[i]->Get_Sliding();
				return COMPARE_STOP;
			}
			else
			{
				*pCellIndex = *m_pNeighbor[i]->Get_Index();
				return COMPARE_MOVE;
			}
		}
	}

	return COMPARE_MOVE;
}

void CCell::Render_Cell(void)
{
	_vec3 vPoint[4];
	vPoint[0] = m_vPoint[POINT_A];
	vPoint[1] = m_vPoint[POINT_B];
	vPoint[2] = m_vPoint[POINT_C];
	vPoint[3] = m_vPoint[POINT_A];

	_matrix matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	for (_ulong i = 0; i < 4; ++i)
	{
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matView);

		if (vPoint[i].z <= 0.1f)
			vPoint[i].z = 0.1f;
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matProj);
	}

	m_pD3DXLine->SetWidth(3.f);

	m_pGraphicDev->EndScene();
	m_pGraphicDev->BeginScene();

	m_pD3DXLine->Begin();

	_matrix matTemp;
	D3DXMatrixIdentity(&matTemp);

	m_pD3DXLine->DrawTransform(vPoint, 4, &matTemp, D3DXCOLOR(0.f, 0.f, 0.f, 1.f)); // 정점 및 선분 렌더

	m_pD3DXLine->End();

	// 정점에 구 렌더
	LPD3DXMESH pSphere; // 구를 만들고 렌더에서의 표시.

	D3DXCreateSphere(m_pGraphicDev, 0.1f, 10, 10, &pSphere, NULL);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTexture(0, nullptr);
	for (_ulong i = 0; i < POINT_END; ++i)
	{
		_matrix matWorld;

		D3DXMatrixTranslation(&matWorld, m_vPoint[i].x, m_vPoint[i].y, m_vPoint[i].z);

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
		
		if (m_bClick[i]) // 클릭했는지 확인해야한다.
		{
			m_mtrl.Diffuse.r = m_mtrl.Ambient.r = 1.f;
			m_mtrl.Diffuse.g = m_mtrl.Ambient.g = 1.f;
			m_mtrl.Diffuse.b = m_mtrl.Ambient.b = 0.f;
			m_mtrl.Diffuse.a = m_mtrl.Ambient.a = 1.f;
		}
		else
		{
			m_mtrl.Diffuse.r = m_mtrl.Ambient.r = 1.f;
			m_mtrl.Diffuse.g = m_mtrl.Ambient.g = 1.f;
			m_mtrl.Diffuse.b = m_mtrl.Ambient.b = 1.f;
			m_mtrl.Diffuse.a = m_mtrl.Ambient.a = 1.f;
		}

		m_pGraphicDev->SetMaterial(&m_mtrl);
		pSphere->DrawSubset(0);
	}

	Safe_Release(pSphere); // 구 사용이 끝나면 반드시 삭제해줄것.

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 삼각형 그리기.
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matTemp);
	m_pTriCol->Render_Buffer();

}

CCell * CCell::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
						const _ulong& dwIndex, 
						const _vec3 * pPointA,
						const _vec3 * pPointB,
						const _vec3 * pPointC, 
						const CELL_OPTION& eOption, const _uint& iGroup)
{
	CCell* pInstance = new CCell(pGraphicDev);

	if (FAILED(pInstance->Ready_Cell(dwIndex, pPointA, pPointB, pPointC, eOption, iGroup)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCell::Free(void)
{
	for (_ulong i = 0; i < LINE_END; ++i)
		Safe_Release(m_pLine[i]);

	Safe_Release(m_pTriCol);
#ifdef _DEBUG
	Safe_Release(m_pD3DXLine);
#endif
	Safe_Release(m_pGraphicDev);
}
