#ifndef NaviMesh_h__
#define NaviMesh_h__

#include "Mesh.h"
#include "Cell.h"

BEGIN(Engine)

class ENGINE_DLL CNaviMesh : public CMesh
{
private:
	explicit CNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CNaviMesh(const CNaviMesh& rhs);
	virtual	~CNaviMesh(void);

public:
	const vector<CCell*>& Get_vecCell();
	const _int&	Get_CurIndex() { return m_vecCell.size() - 1; }
	const Engine::CCell::CELL_OPTION& Get_CellOption(const _int& iIndex) { return m_vecCell[iIndex]->Get_CellOption(); }
	const _int& Get_Group(const _int & iIndex);
public:
	void				SetCurrentIndex(_ulong dwIndex) { m_dwCurrentIdx = dwIndex; }
	void				Set_CellPos(const _vec3& vPos, const CCell::CELL_OPTION& eOption, const _uint& iGroup = 100); // 클릭 및 생성
	void				Set_MovePos(const _vec3& vPos); // 드래그
	void				Set_PointPos(const _vec3& vPrePos, const _vec3& vChanagePos); // 정점 위치 변환.
	const _float&		Set_NaviPosY(const _vec3& vTargetPos);
	void				Set_Group(const _int & iIndex, const _uint & iGroup);

public:
	_bool		Delete_Cell(const _vec3& vPos, _int* pIndex);
	_bool		Get_PickCell(const _vec3& vPos, _int* pIndex);					//Picking 시 인덱스 뽑아오기.
	_bool		Delete_Cell(const _int& iIndex);
	void		Clear_Cell(void);
	void		Clear_Pos(void);
	void		Change_CellOption(const _int & iIndex, const CCell::CELL_OPTION & eOption);
	void		Change_CellOption(const CCell::CELL_OPTION & eDstOption, const CCell::CELL_OPTION & eSrcOption, const _uint& iGroup);
	void		Click_Point(const _vec3& vPos, _vec3* pOutPos);
	_bool		Picking_Sphere(const _vec3& vMousePos, _matrix* pMatWorld);
public:
	HRESULT		Ready_NaviMesh(void);
	void		Render_NaviMesh(void);
	_vec3		MoveOn_NaviMesh(const _vec3* pTargetPos, const _vec3* pTargetDir, const _float& fTimeDelta = 0.f, const _float& fSpeed = 0.f);

public:
	HRESULT		Link_Cell(void);

private:
	vector<CCell*>			m_vecCell;
	_ulong					m_dwCurrentIdx;
	vector<_vec3>			m_vecPos;
public:
	static CNaviMesh*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CResources* Clone(void);
	virtual void		Free(void);
};
END
#endif // NaviMesh_h__
