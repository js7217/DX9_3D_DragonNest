#ifndef ResourcesMgr_h__
#define ResourcesMgr_h__

#include "Engine_Defines.h"
#include "Base.h"

#include "RcCol.h"
#include "TriCol.h"
#include "Texture.h"
#include "RcTex.h"
#include "CubeTex.h"
#include "TerrainTex.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "NaviMesh.h"

BEGIN(Engine)

class ENGINE_DLL CResourcesMgr : public CBase
{
	DECLARE_SINGLETON(CResourcesMgr)

private:
	explicit CResourcesMgr(void);
	virtual ~CResourcesMgr(void);

public:
	HRESULT Reserve_ContainerSize(const _ushort& wSize);
	HRESULT Ready_Buffers(LPDIRECT3DDEVICE9 pGraphicDev,
		const _ushort& wContainerIdx,
		const _tchar* pBufferTag,
		BUFFERID eID, 
		const WORD& wCntX = 0, const WORD& wCntY = 0, const WORD& wItv = 0, const WORD& wDetail = 1);

	HRESULT		Ready_Textures(LPDIRECT3DDEVICE9 pGraphicDev,
		const _ushort& wContainerIdx,
		const _tchar* pTextureTag,
		TEXTURETYPE eType,
		const _tchar* pPath,
		const _uint& iCnt = 1);

	HRESULT		Ready_Meshes(LPDIRECT3DDEVICE9 pGraphicDev, const _ushort& wContainerIdx, const _tchar* pMeshTag, MESHTYPE eType, const _tchar* pFilePath, const _tchar* pFileName);


	void Render_Buffers(const _ushort& wContainerIdx,
		const _tchar* pBufferTag);

private:
	CResources* Find_Resources(const _ushort& wContainerIdx,
		const _tchar* pResourcesTag);

public:
	HRESULT		Delete_Resources(const _ushort& wContainerIdx,
		const _tchar* pResourcesTag); // 리소스 삭제.
private:
	typedef map<const _tchar*, CResources*> MAPRESOURCES;
	MAPRESOURCES*	m_pmapResources;

	_ulong			m_iContainerSize;

public:
	CResources* Clone_Resources(const _ushort& wContainerIdx,
								const _tchar* pResourceTag);


public:
	virtual void Free(void);
};
END
#endif // ResourcesMgr_h__
