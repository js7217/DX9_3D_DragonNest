#ifndef Texture_h__
#define Texture_h__

#include "Resources.h"

BEGIN(Engine)

class ENGINE_DLL CTexture : public CResources
{
protected:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture(void);

public:
	const _uint&		Get_TextureCnt(void) { return m_iTextureCnt; }
public:
	HRESULT Ready_Texture(const _tchar* pPath, TEXTURETYPE eType, const _uint& iCnt);
	void	Set_Texture(LPD3DXEFFECT pEffect, const char* pShaderName, const _uint& iIndex = 0);
	void	Render(const _uint& iIndex = 0);

private:
	vector<IDirect3DBaseTexture9*>	m_vecTexture;
	_uint							m_iTextureCnt;

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _tchar* pPath, TEXTURETYPE eType, const _uint& iCnt = 1);

	virtual CResources* Clone(void);
	virtual void		Free(void);
};
END
#endif // Texture_h__
