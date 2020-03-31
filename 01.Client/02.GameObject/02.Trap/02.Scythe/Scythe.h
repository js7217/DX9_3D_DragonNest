#ifndef Scythe_h__
#define Scythe_h__

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

class CScythe : public Engine::CGameObject
{
private:
	explicit CScythe(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScythe(void);

public:
	virtual HRESULT Ready_Object(const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	float			Random(float min, float max);
private:
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CTransform*		m_pTransCom;
	Engine::CRenderer*		m_pRenderCom;
	Engine::COptimization*	m_pOptimization;
	Engine::CShader*		m_pShaderCom;

	_float				m_fSpeed;
	_bool				m_bDraw = false;
	_float				m_fTimeDelta;

	const _matrix*			m_pParentBoneMatrix;
	_matrix					m_matDist;
public:
	static CScythe*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate);

	virtual void Free(void) override;
};
#endif // Scythe_h__
