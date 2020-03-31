#ifndef Goblin_h__
#define Goblin_h__

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
class CSword;
class CGoblin : public Engine::CGameObject
{
private:
	explicit CGoblin(LPDIRECT3DDEVICE9 pGraphicDev, CPlayer* pPlayer);
	virtual ~CGoblin(void);

public:
	Engine::CTransform*			Get_TransformCom();
	Engine::CDynamicMesh*		Get_MeshCom() { return m_pMeshCom; }
	const wstring&				Get_MeshName() { return m_wstrName; }
public:
	void Set_Pos(const _vec3& vPos);
	void Set_Size(const _vec3& vSize);
	void Set_Rotate(const _vec3& vRotate);
	void Set_NaviCom(Engine::CNaviMesh* pNaviMesh);				// Terrain �� NaviCom�� �Ѱ��ش�.
	void Set_Sword(CSword* pSword) { m_pSword = pSword; }
public:
	virtual HRESULT Ready_Object(const _int& iFlag);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

public:
	virtual void	Set_HitEffect();
	 
private:			//�ʱ�ȭ �Լ�.
	HRESULT			Add_Component(void);
private:
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void			Chase_Target();				// // Ÿ���� ���󰡴� �Լ�.
	void			MotionChange();
	void			MotionEvent();				// �� ��Ǹ����� �̺�Ʈ�� �� ���̴�.
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

	wstring					m_wstrName; // �ش� ��ü�� �޽� �̸�.


	//�׽�Ʈ
	_float					m_fTimeDelta;
	//  �ִϸ��̼� �ӵ�
	_float					m_fAniSpeed;	//  �ִϸ��̼� �ӵ�.
	// ����
	CSword*					m_pSword = nullptr;		// ����
	// �¾�����
	_bool					m_bIsFirstHit = false; // �Ѵ�� ������.
	
public:
	static CGoblin*			Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh, CPlayer* pPlayer);
	static CGoblin*			Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh, CPlayer* pPlayer,
		const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate, const _uint& iGroup);
private:
	virtual void Free(void) override;
};
#endif // Goblin_h__
