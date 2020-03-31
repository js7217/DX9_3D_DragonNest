#ifndef Cell_h__
#define Cell_h__

#include "Engine_Defines.h"
#include "Base.h"

#include "Line.h"
#include "CellTriCol.h"

BEGIN(Engine)

class ENGINE_DLL CCell : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
	enum COMPARE { COMPARE_MOVE, COMPARE_STOP };
	enum CELL_OPTION { NORMAL, TRIGGER, EVENT };
private:
	explicit CCell(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCell(void);

public:
	const _vec3*		Get_Point(POINT eType) const { return &m_vPoint[eType]; }
	CCell*				Get_Neighbor(NEIGHBOR eType) const { return m_pNeighbor[eType]; }
	const _ulong*		Get_Index(void) { return &m_dwIndex; }
	const CELL_OPTION&	Get_CellOption() { return m_eOption; }
	const _uint&		Get_Group() { return m_iGroup; }
public:
	void				Set_Point(POINT eType, const _vec3& vPos);
	void				Set_Neighbor(NEIGHBOR eType, CCell* pNeighbor) { m_pNeighbor[eType] = nullptr; m_pNeighbor[eType] = pNeighbor; }
	void				Set_CellOption(const CELL_OPTION& eOption);
	void				Set_bIsCheck(POINT eType); // 해당 포인트가 클릭이 되었는지 안되었는지 판단.
	void				Set_Group(const _uint& iGroup) { m_iGroup = iGroup; }
public:
	_bool				Picking_Cell(const _vec3& vPos);
	void				Reset_Check();
public:
	HRESULT Ready_Cell(const _ulong& dwIndex,
		const _vec3* pPointA,
		const _vec3* pPointB,
		const _vec3* pPointC,
		const CELL_OPTION& eOption, const _uint& iGroup);

	_bool		Compare_Point(const _vec3* pFirstPoint, const _vec3* pSecondPoint, CCell* pCell);
	COMPARE		Compare(_vec3* pEndPos, _ulong* pCellIndex, _vec3* pSliding, const _vec3* pTargetDir);
	void		Render_Cell(void);

private:
	_vec3			m_vPoint[POINT_END];
	CCell*			m_pNeighbor[NEIGHBOR_END];
	CLine*			m_pLine[LINE_END];
	CCellTriCol*	m_pTriCol;

	LPDIRECT3DDEVICE9	m_pGraphicDev;
	LPD3DXLINE			m_pD3DXLine; // 라인을 그리기 위한 객체.
	_ulong				m_dwIndex = 0;
	CELL_OPTION			m_eOption = NORMAL; // 해당 Cell의 옵션을 나타냄.
	_bool				m_bClick[POINT_END]; // 해당 포인트가 클릭 되었는가 안되었는가 를 판단.
	D3DMATERIAL9		m_mtrl;
	_uint				m_iGroup = 100;		// 기본이 100 (즉 디폴트)
public:
	static CCell*	Create(LPDIRECT3DDEVICE9 pGraphicDev, 
		const _ulong& dwIndex, 
		const _vec3* pPointA, 
		const _vec3* pPointB, 
		const _vec3* pPointC, 
		const CELL_OPTION& eOption, const _uint& iGroup);
	virtual void	Free(void);
};
END
#endif // Cell_h__
