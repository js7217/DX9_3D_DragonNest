#ifndef VIBuffer_h__
#define VIBuffer_h__

#include "Resources.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer : public CResources
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer(void);

public:
	_ulong	Get_TriCnt(void) { return m_dwTriCnt; }

public:
	HRESULT Ready_Buffer(void);
	void	Render_Buffer(void);

protected:
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;

	_ulong		m_dwVtxSize; // 해당 버텍스의 사이즈
	_ulong 		m_dwVtxCnt; // 갯수
	_ulong 		m_dwTriCnt; // 삼각형 갯수
	_ulong 		m_dwVtxFVF; // 해당 버텍스 자료형

	_ulong 		m_dwIdxSize; // 인덱스 사이즈
	D3DFORMAT	m_IdxFmt; // 인덱스 포맷

public:
	virtual void Free(void);
};

END
#endif // VIBuffer_h__
