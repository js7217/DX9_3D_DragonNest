#ifndef Calculator_h__
#define Calculator_h__

#include "Component.h"

BEGIN(Engine)

class CTerrainTex;
class CTransform;
class ENGINE_DLL CCalculator : public CComponent
{
private:
	typedef struct tagOBB
	{
		_vec3 vPoint[8];	// ť�긦 �̷�� ���� 8��
		_vec3 vCenter;		// ť�� ������ ������ ����
		_vec3 vProjAxis[3]; // �������� ť���� �� ���� ���� ����� ����
		_vec3 vAxis[3];		// ť��� ������ ������ ���� �����ϱ� ���� ����
	}OBB;

private:
	explicit CCalculator(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCalculator(void);

public:
	HRESULT Ready_Calculator(void);
	_float	Compute_HeightOnTerrain(const _vec3* pPos,
									const _vec3* pTerrainVtxPos,
									const _ulong& dwCntX,
									const _ulong& dwCntZ);

	bool Picking_OnTerrain(_vec3* pOut, HWND hWnd, const CTerrainTex* pTerrainTexBufferCom, const CTransform* pTerrainTransCom);
	_bool Picking_Sphere(HWND hWnd, _matrix* pMatWorld);
	_bool Collision_AABB(const _vec3* pDestMin, const _vec3* pDestMax, const _matrix* pDestWorld,
		const _vec3* pSourMin, const _vec3* pSourMax, const _matrix* pSourWorld);
	_bool Collision_OBB(const _vec3* pDestMin, const _vec3* pDestMax, const _matrix* pDestWorld,
		const _vec3* pSourMin, const _vec3* pSourMax, const _matrix* pSourWorld);
private:
	void Set_Point(OBB* pObb, const _vec3* pMin, const _vec3* pMax);
	void Set_Axis(OBB* pObb);

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	map<_float, _vec3>			m_mapTile;

public:
	static CCalculator* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);
};
END
#endif // Calculator_h__
