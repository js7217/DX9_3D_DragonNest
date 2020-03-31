#ifndef Layer_h__
#define Layer_h__

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CLayer : public CBase
{
private:
	explicit CLayer(void);
	virtual ~CLayer(void);

public:
	virtual HRESULT Ready_Layer(void);
	virtual _int	Update_Layer(const _float& fTimeDelta);
	virtual void	Render_Layer(void);

public:
	const _uint&	Get_GroupCount() { return m_iGroupCount; }
public:
	HRESULT			Add_GameObject(const _tchar * pObjtag, CGameObject* pGameObject);
	CComponent*		Get_Component(const _tchar* pObjtag, const _tchar* ComponentKey, COMPONENTID eID);
	HRESULT			Delete_GameObject(const _tchar* pObjtag);
	HRESULT			Erase_GameObject(const _tchar* pObjtag, CGameObject* pGameObject);
	_bool			Empty_GameObject(const _tchar* pObjTag);
public:
	_bool			CollisionObject(const _tchar * pObjtag, const _matrix& matSphere, const _float& fRadius, const CGameObject::HIT_OPTION& eHitOption = CGameObject::HIT_NORMAL);
	HRESULT			Reset_HitObject(const _tchar* pObjtag); // 몬스터 레이어를 아예 따로 두는 방법도 고려해보자/..
	void			Open_Door(const _uint& iGroup);
	_uint			Count_Group(const _uint& iGroup);

private:
	map<const _tchar*, list<CGameObject*>>			m_mapObject;
	_uint											m_iGroupCount = 0;
public:
	static CLayer*	Create(void);
	virtual void	Free(void);
};

END
#endif // Layer_h__
