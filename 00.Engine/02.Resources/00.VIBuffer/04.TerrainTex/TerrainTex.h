#ifndef TerrainTex_h__
#define TerrainTex_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTex : public CVIBuffer
{
private:
	explicit CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainTex(const CTerrainTex& rhs);
	virtual ~CTerrainTex(void);

public:
	const _vec3*		Get_VtxPos(void) const { return m_pPos; }
	//_ulong				Get_VtxCntX(void) const { return m_iH.biWidth; }
	//_ulong				Get_VtxCntZ(void) const { return m_iH.biHeight; }
	_ulong				Get_VtxCntX(void) const { return m_dwCntX; }
	_ulong				Get_VtxCntZ(void) const { return m_dwCntZ; }
	void				Copy_Index(Engine::INDEX32* pIndex, const _ulong& dwTriCnt);
public:
	void				Set_Cnt(const _ulong& dwCntX, const _ulong& dwCntZ) { m_dwCntX = dwCntX; m_dwCntZ = dwCntZ; }
public:
	HRESULT	Ready_Buffer(const _ulong& dwCntX,
		const _ulong& dwCntZ,
		const _ulong& dwVtxItv,
		const _ulong& dwDetail);

private:
	HANDLE				m_hFile;
	// Height맵 사용시 사용하는 가로 세로
	BITMAPFILEHEADER	m_fH;
	BITMAPINFOHEADER	m_iH;
	// Tool과 같이 커스텀한 크기의 가로 세로
	_ulong				m_dwCntX;
	_ulong				m_dwCntZ;

	_vec3*				m_pPos;
	_bool				m_bClone;


public:
	static CTerrainTex*		Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _ulong& dwCntX,
		const _ulong& dwCntZ,
		const _ulong& dwVtxItv,
		const _ulong& dwDetail);

	virtual CResources*		Clone(void);
	virtual void			Free(void);
};
END
#endif // TerrainTex_h__
