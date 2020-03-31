#ifndef Camera_h__
#define Camera_h__

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CTransform;
}

class CCamera : public Engine::CGameObject
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
public:
	virtual ~CCamera(void);

protected:
	void MakeViewMatrix(const _vec3* pEye,
		const _vec3* pAt, const _vec3* pUp);

	void MakeProjectionMatrix(const _float& fFovY, const _float& fAspect,
		const _float& fNear, const _float& fFar);

protected: // 카메라는 결국 뷰행렬, 투영행렬을 만든다.
	Engine::CTransform*		m_pTransCom;
	_matrix					m_matView;
	_matrix					m_matProj;
	_vec3					m_vEye;
	_vec3					m_vAt;
};
#endif // Camera_h__
