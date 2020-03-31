#ifndef Shovel_Bullet_h__
#define Shovel_Bullet_h__

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CCalculator;
	class CRenderer;
	class CTransform;
	class CStaticMesh;
	class CBoxCollider;
	class COptimization;
	class CDynamicMesh;
	class CShader;
}

class CShovel_Bullet : public Engine::CGameObject
{
private:
	explicit CShovel_Bullet(LPDIRECT3DDEVICE9 pGarphicDev);
	virtual ~CShovel_Bullet(void);

public:
	HRESULT Ready_Object(const _vec3& vThrowLook, const _vec3& vStartPos, const _float& fRotY);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT Add_Component();
	HRESULT	SetUp_ConstantTable(LPD3DXEFFECT pEffect);


private:
	Engine::CTransform*			m_pTransCom;
	Engine::CRenderer*			m_pRenderCom;
	Engine::CStaticMesh*		m_pMeshCom;
	Engine::CShader*			m_pShaderCom;
	

	_bool						m_bIsDead = false;
	_float						m_fThrowTimer = 3.f;
	_vec3						m_vThrowLook;
public:
	static CShovel_Bullet*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vThrowLook, const _vec3& vStartPos, const _float& fRotY);

private:
	virtual void Free(void) override;
};
#endif // Shovel_Bullet_h__
