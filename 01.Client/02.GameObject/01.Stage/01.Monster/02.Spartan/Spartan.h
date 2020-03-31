#ifndef Spartan_h__
#define Spartan_h__

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
class CShovel;
class CSpartan : public Engine::CGameObject
{
private:
	explicit CSpartan(LPDIRECT3DDEVICE9 pGraphicDev, CPlayer* pPlayer);
	virtual ~CSpartan(void);

public:
	Engine::CTransform*			Get_TransformCom();
	Engine::CDynamicMesh*		Get_MeshCom() { return m_pMeshCom; }
	const wstring&				Get_MeshName() { return m_wstrName; }
public:
	void Set_Pos(const _vec3& vPos);
	void Set_Size(const _vec3& vSize);
	void Set_Rotate(const _vec3& vRotate);
	void Set_NaviCom(Engine::CNaviMesh* pNaviMesh);				// Terrain 의 NaviCom을 넘겨준다.
	void Set_Sword(CShovel* pSword) { m_pSword = pSword; }
public:
	virtual HRESULT Ready_Object(const _int& iFlag);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;
	 
public:
	virtual void	Set_HitEffect();

private:			//초기화 함수.
	HRESULT			Add_Component(void);
private:
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void			Chase_Target();				// // 타겟을 따라가는 함수.
	void			MotionChange();
	void			MotionEvent();				// 각 모션마다의 이벤트를 줄 것이다.
private:
	Engine::CTransform*		m_pTransCom;
	Engine::CRenderer*		m_pRenderCom;
	Engine::CCalculator*	m_pCalculatorCom;
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CNaviMesh*		m_pNaviMeshCom;
	Engine::CShader*		m_pShaderCom;
	Engine::CBoneCollider*	m_pBoneColliderCom;

	CPlayer*				m_pTarget;

	_bool					m_bIsColl;
	_bool					m_bIsDead;
	_int					m_iFlag;
	_float					m_fSpeed;
	_vec3					m_vDir;

	wstring					m_wstrName; // 해당 객체의 메쉬 이름.


	//테스트
	_float					m_fTimeDelta;
	//  애니메이션 속도
	_float					m_fAniSpeed;	//  애니메이션 속도.
	// 무기
	CShovel*				m_pSword = nullptr;		// 무기
	// 맞았을때
	_bool					m_bIsFirstHit = false; // 한대라도 맞으면.
	
public:
	static CSpartan*			Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh, CPlayer* pPlayer);
	static CSpartan*			Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh, CPlayer* pPlayer,
		const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate, const _uint& iGroup);
private:
	virtual void Free(void) override;
};
#endif // Spartan_h__
