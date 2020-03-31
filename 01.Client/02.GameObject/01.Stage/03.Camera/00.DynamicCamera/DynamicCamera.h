#ifndef DynamicCamera_h__
#define DynamicCamera_h__

#include "Defines.h"
#include "Camera.h"

class CDynamicCamera : public CCamera
{
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;

private:
	HRESULT AddComponent(void);
	HRESULT Key_Input(const _float& fTimeDelta);
	HRESULT Make_TransformCom(void);
private:
	_vec3	m_vPreMouse;
	_vec3	m_vCurMouse;
	_vec3	m_vDir;
public:
	static CDynamicCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);
};

#endif // DynamicCamera_h__
