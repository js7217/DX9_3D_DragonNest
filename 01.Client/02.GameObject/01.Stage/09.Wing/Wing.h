#ifndef Wing_h__
#define Wing_h__

#include "Defines.h"
#include "GameObject.h"
#include "Cell.h"
namespace Engine
{
	class CTransform;
	class CRenderer;
	class CDynamicMesh;
	class CShader;
}

class CWing : public Engine::CGameObject
{
public:
	enum WING_MODE {DISPLAY, EQUIP};
private:
	explicit CWing(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWing(void);

public:
	const wstring& Get_MeshName() { return m_wstrName; }
	const _vec3&	Get_Pos();
	const _vec3&	Get_Size();
	const _vec3&	Get_Rotate();
public:
	void Set_Pos(const _vec3& vPos);
	void Set_Size(const _vec3& vSize);
	void Set_Rotate(const _vec3& vRotate);
	void Set_Dead() { m_bIsDead = true; }

public:
	virtual HRESULT Ready_Object();
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void			Change_WingMode(); // 장착 상태인지 전시 상태인지 판단.
private:
	Engine::CTransform*		m_pTransCom;
	Engine::CRenderer*		m_pRenderCom;
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CShader*		m_pShaderCom;

	wstring					m_wstrName; // 해당 객체의 메쉬 이름.
	_bool					m_bIsDead;

	const _matrix*		m_pParentBoneMatrix;
	const _matrix*		m_pParentWorldMatrix;

	WING_MODE			m_eMode;

public:
	static CWing*			Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CWing*			Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate);
private:
	virtual void Free(void) override;
};
#endif // Wing_h__
