#ifndef LightMgr_h__
#define LightMgr_h__

#include "Engine_Defines.h"
#include "Base.h"

#include "Light.h"

BEGIN(Engine)
class ENGINE_DLL CLightMgr final : public CBase
{
	DECLARE_SINGLETON(CLightMgr)
private:
	explicit CLightMgr();
	virtual ~CLightMgr();
public:
	void Set_LightIndex(const _uint& iIndex) { m_iIndex = iIndex; }
public:
	HRESULT Add_Light(LPDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9& LightInfo, const _uint& iIndex);
	HRESULT Render_Light_Manager(LPD3DXEFFECT pEffect);
private:
	list<CLight*>			m_LightList;
	typedef list<CLight*>	LIGHTLIST;
	map<_uint, LIGHTLIST>	m_mapLight;
	_uint					m_iIndex;
public:
	virtual void Free();
};
END
#endif // LightMgr_h__
