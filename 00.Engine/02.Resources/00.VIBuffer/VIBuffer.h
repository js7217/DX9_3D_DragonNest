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

	_ulong		m_dwVtxSize; // �ش� ���ؽ��� ������
	_ulong 		m_dwVtxCnt; // ����
	_ulong 		m_dwTriCnt; // �ﰢ�� ����
	_ulong 		m_dwVtxFVF; // �ش� ���ؽ� �ڷ���

	_ulong 		m_dwIdxSize; // �ε��� ������
	D3DFORMAT	m_IdxFmt; // �ε��� ����

public:
	virtual void Free(void);
};

END
#endif // VIBuffer_h__
