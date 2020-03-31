#ifndef HitEffect_h__
#define HitEffect_h__

#include "Defines.h"
#include "GameObject.h"
namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CRenderer;
	class CShader;
}

class CHitEffect : public Engine::CGameObject
{
private:
	explicit CHitEffect(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pResourceName);
	virtual ~CHitEffect(void);

public:
	void	Set_Pos(const _vec3& vPos);
public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		AddComponent(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT		Update_Option(const _float& fTimeDelta);
	HRESULT		Set_BillBoard();
private:
	Engine::CRcTex*				m_pBufferCom;
	Engine::CTransform*			m_pTransCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CRenderer*			m_pRenderCom;
	Engine::CShader*			m_pShaderCom;

	_float						m_fAlpha;
	_float						m_fLifeTime;
	_bool						m_bIsDead;

	const _tchar*				m_pResourceName;
	_float						m_fAddPosY;				// Y축 이동 증가량.
public:
	static CHitEffect*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _tchar* pResourceName);

private:
	virtual void Free(void) override;

};


#endif // HitEffect_h__
