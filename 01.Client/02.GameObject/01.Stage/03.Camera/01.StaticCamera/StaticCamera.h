#ifndef StaticCamera_h__
#define StaticCamera_h__

#include "Defines.h"
#include "Camera.h"

namespace Engine
{
	class CTransform;
	class CNaviMesh;
}
class CPlayer;
class CStaticCamera : public CCamera
{
private:
	explicit CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh);
	virtual ~CStaticCamera(void);

public:
	void Set_Target(CPlayer* pTarget);
public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;

private:
	HRESULT			Key_Input(const _float& fTimeDelta);
	HRESULT			Make_TransformCom(void);
	void			Fix_Mouse(void);
	void			Rotate_TargetMouse(void);
	void			Change_CamMode();
private:
	CPlayer*				m_pTarget;
	Engine::CTransform*		m_pCamTarget;
	Engine::CNaviMesh*		m_pNaviMeshCom;


	_float					m_fTargetDist;
	_float					m_fCamAngle;
	_float					m_fCamSpeed;
	_bool					m_bIsClick;
	_bool					m_bMouseFix;
	_float					m_fCamAngleY;

public:
	static CStaticCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev, CPlayer* pTarget, Engine::CNaviMesh* pNaviMesh);
};

#endif // StaticCamera_h__
