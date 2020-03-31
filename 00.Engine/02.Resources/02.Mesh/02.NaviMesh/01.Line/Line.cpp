#include "Line.h"

USING(Engine)

CLine::CLine(void)
{
}

CLine::~CLine(void)
{
}

HRESULT CLine::Ready_Line(const _vec2 * pPointA, const _vec2 * pPointB)
{
	m_vPoint[POINT_START]	= *pPointA;
	m_vPoint[POINT_FINISH]	= *pPointB;

	m_vDirection = m_vPoint[POINT_FINISH] - m_vPoint[POINT_START]; //  선분을 담당하는 벡터
	m_vNormal = _vec2(m_vDirection.y * -1.f, m_vDirection.x); // 노말벡터 만들기(2차원)
	D3DXVec2Normalize(&m_vNormal, &m_vNormal);

	return S_OK;
}

CLine::COMPARE CLine::Compare(const _vec2 * pEndPos, const _vec3* pTargetDir)
{
	_vec2 vDest = *pEndPos - m_vPoint[POINT_START];
	D3DXVec2Normalize(&vDest, &vDest);

	float fDot = D3DXVec2Dot(&vDest, &m_vNormal);

	if (0 <= fDot)
	{
		return COMPARE_OUT;
	}
	else
		return COMPARE_IN;
}

_vec2 CLine::Make_SlidingVector(const _vec2 * pEndPos, const _vec3* pTargetDir)
{
	// 슬라이딩 벡터 제작.
	_vec2 vDir = _vec2(pTargetDir->x, pTargetDir->z);
	D3DXVec2Normalize(&vDir, &vDir);

	_vec2 vSliding = *pEndPos - D3DXVec2Dot(pEndPos, &m_vNormal) * m_vNormal;

	float fDot = D3DXVec2Dot(&vDir, &vSliding);

	if (0 >= fDot)
		vSliding *= -1.f;

	D3DXVec2Normalize(&vSliding, &vSliding);

	return vSliding;
}

; CLine * CLine::Create(const _vec2 * pPointA, const _vec2 * pPointB)
{
	CLine* pInstance = new CLine;

	if (FAILED(pInstance->Ready_Line(pPointA, pPointB)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLine::Free(void)
{
}
