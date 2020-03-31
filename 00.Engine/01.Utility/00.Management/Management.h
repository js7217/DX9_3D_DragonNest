#ifndef Management_h__
#define Management_h__

#include "Scene.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit CManagement(void);
	virtual ~CManagement(void);

public:
	HRESULT	Ready_ShaderFile(LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT SetUp_CurrentScene(CScene* pCurScene);
	_int	Update_Scene(const _float& fTimeDelta);
	void	Render_Scene();

public:
	CComponent*		Get_Component(const _tchar* pLayertag,
								  const _tchar* pObjtag,
								  const _tchar* ComponentKey,
								  COMPONENTID eID);

	CLayer*			Get_Layer(const _tchar* pLayertag);

	HRESULT			Delete_GameObject(const _tchar* pLayertag, const _tchar* pObjtag);
	HRESULT			Erase_GameObject(const _tchar* pLayertag, const _tchar* pObjtag, CGameObject* pGameObject);
private:
	CScene*		m_pCurrentScene;

public:
	virtual void Free(void);
};
END
#endif // Management_h__
