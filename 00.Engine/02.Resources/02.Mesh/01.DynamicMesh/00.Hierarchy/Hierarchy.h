#ifndef Hierarchy_h__
#define Hierarchy_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class CHierarchy : public ID3DXAllocateHierarchy
{
private:
	explicit CHierarchy(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath);
	virtual ~CHierarchy(void);

public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name,
		LPD3DXFRAME *ppNewFrame);

	STDMETHOD(CreateMeshContainer)(THIS_
		LPCSTR Name,
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,
		CONST D3DXEFFECTINSTANCE *pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD *pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER *ppNewMeshContainer);


	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);


	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree);

private:
	void Allocate_Name(char** ppName, const char* pFrameName);

private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	const _tchar*		m_pPath;

public:
	static CHierarchy* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath);
public:
	_ulong Release(void);
};

END
#endif // Hierarchy_h__
