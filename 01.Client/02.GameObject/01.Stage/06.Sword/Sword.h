#ifndef Sword_h__
#define Sword_h__

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

class CSword : public Engine::CGameObject
{
private:
	explicit CSword(LPDIRECT3DDEVICE9 pGarphicDev);
	virtual ~CSword(void);

public:
	HRESULT Ready_Object(Engine::CDynamicMesh* pTargetMeshCom, Engine::CTransform* pTargetTransCom, const _tchar* pMeshName);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT Add_Component(const _tchar* pMeshName);
	HRESULT	SetUp_ConstantTable(LPD3DXEFFECT pEffect);


private:
	Engine::CTransform*			m_pTransCom;
	Engine::CRenderer*			m_pRenderCom;
	Engine::CStaticMesh*		m_pMeshCom;
	Engine::CShader*			m_pShaderCom;


	const _matrix*		m_pParentBoneMatrix;
	const _matrix*		m_pParentWorldMatrix;
	
	Engine::CDynamicMesh*		m_pTargetMeshCom;
public:
	static CSword*		Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CDynamicMesh* pTargetMeshCom, Engine::CTransform* pTargetTransCom, const _tchar* pMeshName);

private:
	virtual void Free(void) override;
};
#endif // Sword_h__
