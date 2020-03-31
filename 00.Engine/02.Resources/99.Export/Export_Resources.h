#ifndef Export_Resources_h__
#define Export_Resources_h__

#include "Engine_Defines.h"
#include "ResourcesMgr.h"

BEGIN(Engine)

// ResourcesMgr
// Get
// Set
// public
inline HRESULT		Reserve_ContainerSize(const _ushort& wSize);
inline HRESULT		Ready_Buffers(LPDIRECT3DDEVICE9 pGraphicDev, const _ushort& wContainerIdx, const _tchar* pBufferTag, BUFFERID eID, const WORD& wCntX = 0, const WORD& wCntY = 0, const WORD& wItv = 0, const WORD & wDetail = 1);
inline HRESULT		Ready_Textures(LPDIRECT3DDEVICE9 pGraphicDev, const _ushort& wContainerIdx, const _tchar* pTextureTag, TEXTURETYPE eType, const _tchar* pPath, const _uint& iCnt = 1);
inline void			Render_Buffers(const _ushort& wContainerIdx, const _tchar* pBufferTag);
inline HRESULT		Ready_Meshes(LPDIRECT3DDEVICE9 pGraphicDev, const _ushort& wContainerIdx, const _tchar* pMeshTag, MESHTYPE eType, const _tchar* pFilePath, const _tchar* pFileName);
inline CResources*	Clone_Resources(const _ushort& wContainerIdx, const _tchar* pResourceTag);
inline 	HRESULT		Delete_Resources(const _ushort& wContainerIdx, const _tchar* pResourcesTag);
// Release
inline void Release_Resources(void);


#include "Export_Resources.inl"
END
#endif // Export_Resources_h__
