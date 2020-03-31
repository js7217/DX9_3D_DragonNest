#ifndef Dust_h__
#define Dust_h__

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

class CDust : public Engine::CGameObject
{
public:
	enum DUST_OPTION { WALK, ROLLING, JUMP };
private:
	explicit CDust(LPDIRECT3DDEVICE9 pGraphicDev, DUST_OPTION eDustOption);
	virtual ~CDust(void);

public:
	void	Set_Pos(const _vec3& vPos);
public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		AddComponent(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT		Ready_Option();
	HRESULT		Update_Option(const _float& fTimeDelta);
	HRESULT		SetUp_ResourceName();
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
	DUST_OPTION					m_eDustOption;			// 먼지들의 옵션
	_float						m_fAddPosY;				// Y축 이동 증가량.
public:
	static CDust*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, DUST_OPTION eDustOption);

private:
	virtual void Free(void) override;

};


#endif // Dust_h__
