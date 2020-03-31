#ifndef Logo_h__
#define Logo_h__

#include "Defines.h"
#include "Scene.h"

#include "BackGround.h"
#include "TestMonster.h"
#include "Loading.h"
class CLogo : public Engine::CScene
{
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int	Update_Scene(const _float& fTimeDelta) override;
	virtual void	Render_Scene(void) override;

private:
	CLoading* m_pLoading;
private:
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT			Ready_Resources(void);
public:
	static CLogo*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free(void);
};
#endif // Logo_h__
