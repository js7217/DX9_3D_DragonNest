#ifndef Door_h__
#define Door_h__

#include "Defines.h"
#include "GameObject.h"
#include "Cell.h"

namespace Engine
{
	class CCalculator;
	class CTransform;
	class CRenderer;
	class CDynamicMesh;
	class CNaviMesh;
	class CShader;
	class CBoneCollider;
}

class CPlayer;
class CStone;
class CDoor : public Engine::CGameObject
{
private:
	explicit CDoor(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDoor(void);

public:
	void Set_Pos(const _vec3& vPos);
	void Set_Size(const _vec3& vSize);
	void Set_Rotate(const _vec3& vRotate);

public:
	Engine::CTransform* Get_TransformCom();
	const wstring& Get_MeshName() { return m_wstrName; }

public:
	virtual HRESULT Ready_Object();
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;
	 
private:			//초기화 함수.
	HRESULT			Add_Component(void);
private:
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void			MotionChange();
	void			MotionEvent();				// 각 모션마다의 이벤트를 줄 것이다.
	void			CreateStone(const _float& fTimeDelta);
private:
	Engine::CTransform*		m_pTransCom;
	Engine::CRenderer*		m_pRenderCom;
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CShader*		m_pShaderCom;

	wstring					m_wstrName; // 해당 객체의 메쉬 이름.

	_float					m_fTimeDelta;


	//Stone
	_float					m_fStoneCreateTimer = 0.f;
	_bool					m_bIsCreateStone = false;
public:
	static CDoor*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate, const _uint& iGroup);
private:
	virtual void Free(void) override;
};
#endif // Door_h__
