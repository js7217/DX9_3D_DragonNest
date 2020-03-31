#ifndef Stone_h__
#define Stone_h__

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CDynamicMesh;
	class COptimization;
	class CShader;
}

class CStone : public Engine::CGameObject
{
private:
	explicit CStone(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStone(void);

public:
	virtual HRESULT Ready_Object(const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void			Go_Straight(const _float& fTimeDelta);
	float Random(float min, float max);
private:
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CTransform*		m_pTransCom;
	Engine::CRenderer*		m_pRenderCom;
	Engine::COptimization*	m_pOptimization;
	Engine::CShader*		m_pShaderCom;

	_vec3				m_vDir;
	_float				m_fSpeed;
	_bool				m_bDraw = false;
	_bool				m_bIsDead = false;
	_float				m_fTimeDelta;
	_float				m_fTimer;
	_float				m_fDustTimer = 0.3f;
public:
	static CStone*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate);

	virtual void Free(void) override;
};
#endif // Stone_h__
