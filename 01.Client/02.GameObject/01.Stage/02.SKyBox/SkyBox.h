#ifndef SkyBox_h__
#define SkyBox_h__

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CCubeTex;
	class CTexture;
	class CTransform;
	class CRenderer;
	class CShader;
}

class CSkyBox : public Engine::CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSkyBox(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		AddComponent(void);
	HRESULT		Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	Engine::CCubeTex*			m_pBufferCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CTransform*			m_pTransCom;
	Engine::CRenderer*			m_pRenderCom;
	Engine::CShader*			m_pShaderCom;

public:
	static CSkyBox*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};


#endif // SkyBox_h__
