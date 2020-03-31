#include "stdafx.h"
#include "Camera.h"

#include "Export_Function.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pTransCom(nullptr)
	, m_vEye(0.f, 0.f, 0.f), m_vAt(0.f, 0.f, 0.f)
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}

CCamera::~CCamera(void)
{

}

void CCamera::MakeViewMatrix(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp)
{
	D3DXMatrixLookAtLH(&m_matView, pEye, pAt, pUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
}

void CCamera::MakeProjectionMatrix(const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	D3DXMatrixPerspectiveFovLH(&m_matProj, fFovY, fAspect, fNear, fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}
