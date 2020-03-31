#ifndef Tree_h__
#define Tree_h__

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
	class CShader;
}

class CTree : public Engine::CGameObject
{
private:
	explicit CTree(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTree(void);

public:
	HRESULT Ready_Object(const _uint& iFlag) ;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void		Set_OnTerrain(void);
	_bool		Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag);


private:
	Engine::CTransform*			m_pTransCom;
	Engine::CRenderer*			m_pRenderCom;
	Engine::CCalculator*		m_pCalculatorCom;
	Engine::CStaticMesh*		m_pMeshCom;
	Engine::CBoxCollider*		m_pColliderCom;
	Engine::COptimization*		m_pOptimizationCom;
	Engine::CShader*			m_pShaderCom;

	_vec3				m_vDir;
	_float				m_fSpeed;
	_uint				m_iFlag;
	_bool				m_bCollision;
	_bool				m_bDraw = false;

public:
	static CTree*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

private:
	virtual void Free(void) override;
};

#endif // Tree_h__
