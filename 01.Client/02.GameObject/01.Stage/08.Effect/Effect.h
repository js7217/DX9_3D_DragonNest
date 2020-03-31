#ifndef Effect_h__
#define Effect_h__

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CRenderer;
}

class CEffect : public Engine::CGameObject
{
private:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		AddComponent(void);

private:
	Engine::CRcTex*				m_pBufferCom;
	Engine::CTransform*			m_pTransCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CRenderer*			m_pRenderCom;

	_float						m_fFrame;

public:
	static CEffect*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};


#endif // Effect_h__
