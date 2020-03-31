#ifndef Target_h__
#define Target_h__

#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CTarget final : public CBase
{
private:
	explicit CTarget(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CTarget() = default;
public:
	HRESULT Ready_Target(_uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor);
	HRESULT Clear_Target();
	HRESULT SetUp_OnGraphicDev(_ulong dwIndex);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName);
	HRESULT Release_OnGraphicDev(_ulong dwIndex);

#ifdef _DEBUG
	HRESULT Ready_Debug_Buffer(_float fStartX, _float fStartY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_Buffer();
#endif

private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;
	LPDIRECT3DTEXTURE9		m_pTarget_Texture = nullptr;
	LPDIRECT3DSURFACE9		m_pTarget_Surface = nullptr;
	LPDIRECT3DSURFACE9		m_pOld_Surface = nullptr;
private:
	D3DXCOLOR				m_ClearColor;
#ifdef _DEBUG
private:
	LPDIRECT3DVERTEXBUFFER9	m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9	m_pIB = nullptr;
#endif

public:
	static CTarget* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor);
	virtual void Free();
};

END
#endif // Target_h__
