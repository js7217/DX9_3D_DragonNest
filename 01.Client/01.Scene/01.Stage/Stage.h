#ifndef Stage_h__
#define Stage_h__

#include "Defines.h"
#include "Scene.h"

#include "Player.h"
#include "SkyBox.h"
#include "DynamicCamera.h"
#include "StaticCamera.h"
#include "Terrain.h"
#include "Light.h"
#include "Effect.h"
#include "StaticObject.h"
#include "Wing.h"
#include "TinyDragon.h"
#include "Shortcuts.h"
#include "FaceUI.h"
#include "HpBar.h"
#include "MpBar.h"
#include "Bar_Back.h"
#include "Goblin.h"
#include "Spartan.h"
#include "Sword.h"
#include "Orge.h"
#include "Shovel.h"
#include "Door.h"
#include "Scythe.h"
#include "Thorn.h"
#include "MouseUI.h"

class CStage : public Engine::CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int	Update_Scene(const _float& fTimeDelta) override;
	virtual void	Render_Scene(void) override;

private:
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT			Ready_Resources(void);
	HRESULT			Ready_Camera(void);
	HRESULT			Ready_Light(void);
	HRESULT			Ready_ProtoType(void);

private:
	Engine::CGameObject*			Load_Terrain(Engine::CLayer* pLayer);
	HRESULT							Load_Mesh(Engine::CLayer * pLayer, Engine::CGameObject* pTerrain, CPlayer* pPlayer);
private:
	Engine::CLight*		m_pLight = nullptr;
	Engine::CNaviMesh*	m_pNaviMesh = nullptr;
	_float				m_pTimer = 0.f;
public:
	static CStage*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free(void);
};
#endif // Stage_h__
