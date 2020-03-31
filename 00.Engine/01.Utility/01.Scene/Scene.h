#ifndef Scene_h__
#define Scene_h__

#include "Layer.h"
#include "Renderer.h"
BEGIN(Engine)

class ENGINE_DLL CScene : public CBase
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene(void);

public:
	virtual HRESULT Ready_Scene(void);
	virtual _int	Update_Scene(const _float& fTimeDelta);
	virtual void	Render_Scene(void);

public:
	CComponent*		Get_Component(const _tchar* pLayertag, 
								  const _tchar* pObjtag,
								  const _tchar* ComponentKey,
								  COMPONENTID eID);

	CLayer*			Get_Layer(const _tchar* pLayertag);
public:
	HRESULT			Delete_GameObject(const _tchar* pLayertag, const _tchar* pObjtag);
	HRESULT			Erase_GameObject(const _tchar* pLayertag, const _tchar* pObjtag, CGameObject* pGameObject);
protected:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	map<const _tchar*, CLayer*> m_mapLayer;

public:
	virtual void Free(void);
};

END
#endif // Scene_h__
