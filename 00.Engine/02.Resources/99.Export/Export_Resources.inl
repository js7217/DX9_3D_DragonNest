// ResourcesMgr
// Get
HRESULT		Ready_Textures(LPDIRECT3DDEVICE9 pGraphicDev,
	const _ushort& wContainerIdx,
	const _tchar* pTextureTag,
	TEXTURETYPE eType,
	const _tchar* pPath,
	const _uint& iCnt)
{
	return CResourcesMgr::GetInstance()->Ready_Textures(pGraphicDev, wContainerIdx, pTextureTag, eType, pPath, iCnt);
}
// Set
// Public
HRESULT		Reserve_ContainerSize(const _ushort& wSize)
{
	return CResourcesMgr::GetInstance()->Reserve_ContainerSize(wSize);
}
HRESULT		Ready_Buffers(LPDIRECT3DDEVICE9 pGraphicDev, const _ushort& wContainerIdx, const _tchar* pBufferTag, BUFFERID eID, const WORD& wCntX, const WORD& wCntY, const WORD& wItv, const WORD & wDetail)
{
	return CResourcesMgr::GetInstance()->Ready_Buffers(pGraphicDev, wContainerIdx, pBufferTag, eID, wCntX, wCntY, wItv, wDetail);
}

void		Render_Buffers(const _ushort& wContainerIdx, const _tchar* pBufferTag)
{
	CResourcesMgr::GetInstance()->Render_Buffers(wContainerIdx, pBufferTag);
}

HRESULT		Ready_Meshes(LPDIRECT3DDEVICE9 pGraphicDev, const _ushort& wContainerIdx, const _tchar* pMeshTag, MESHTYPE eType, const _tchar* pFilePath, const _tchar* pFileName)
{
	return CResourcesMgr::GetInstance()->Ready_Meshes(pGraphicDev, wContainerIdx, pMeshTag, eType, pFilePath, pFileName);
}

CResources*		Clone_Resources(const _ushort& wContainerIdx, const _tchar* pResourceTag)
{
	return CResourcesMgr::GetInstance()->Clone_Resources(wContainerIdx, pResourceTag);
}

HRESULT		Delete_Resources(const _ushort& wContainerIdx, const _tchar* pResourcesTag)
{
	return CResourcesMgr::GetInstance()->Delete_Resources(wContainerIdx, pResourcesTag);
}
// Release
void Release_Resources(void)
{
	CResourcesMgr::GetInstance()->DestroyInstance();
}