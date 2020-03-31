#ifndef Renderer_h__
#define Renderer_h__

#include "Engine_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CTarget_Manager;
class ENGINE_DLL CRenderer : public CComponent
{
private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRenderer(void);
public:
	HRESULT	Clear_Renderer();
public:
	HRESULT Ready_Component();
	void Add_RenderGroup(RENDERID eGroup, CGameObject* pGameObject);
	void Render_GameObject();

private:
	void Render_Priority();
	HRESULT Render_Shadow();
	void Render_NoneAlpha();
	void Render_Alpha();
	void Render_UI();
private:
	HRESULT Render_Deferred();
	HRESULT Render_LightAcc();
	HRESULT Render_Blend();

private:
	list<CGameObject*>			m_RenderGroup[RENDER_END];
	typedef list<CGameObject*>	RENDERLIST;
	LPDIRECT3DDEVICE9			m_pGraphicDev;

private:
	CTarget_Manager*			m_pTarget_Manager = nullptr;
	CShader*					m_pShader_LightAcc = nullptr;
	CShader*					m_pShader_Blend = nullptr;
private:
	LPDIRECT3DVERTEXBUFFER9	m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9	m_pIB = nullptr;

public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();
	virtual	void Free(void);
};

END
#endif // Renderer_h__
