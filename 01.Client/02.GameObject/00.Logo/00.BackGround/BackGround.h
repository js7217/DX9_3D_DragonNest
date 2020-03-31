#ifndef BackGround_h__
#define BackGround_h__

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CTransform;
	class CRenderer;
	class CShader;
}

class CBackGround : public Engine::CGameObject
{
private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBackGround(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		AddComponent(void);
	void		Key_Input(const _float& fTimeDelta);
private:
	Engine::CRcTex*				m_pBufferCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CTransform*			m_pTransCom;
	Engine::CRenderer*			m_pRenderCom;
	Engine::CShader*			m_pShaderCom;
public:
	static CBackGround*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};


#endif // BackGround_h__
