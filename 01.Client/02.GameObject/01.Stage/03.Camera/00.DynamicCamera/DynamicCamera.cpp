#include "stdafx.h"
#include "DynamicCamera.h"

#include "Export_Function.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev)
{

}

CDynamicCamera::~CDynamicCamera(void)
{

}

HRESULT CDynamicCamera::Ready_Object(void)
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_vPreMouse = m_vCurMouse = _vec3(float(WINCX / 2.f), float(WINCY / 2.f), 0.f); // 마우스 초기화
	return S_OK;
}

_int CDynamicCamera::Update_Object(const _float& fTimeDelta)
{
	FAILED_CHECK_RETURN(Key_Input(fTimeDelta), -1);
	FAILED_CHECK_RETURN(Make_TransformCom(), -1);
	Engine::CGameObject::Update_Object(fTimeDelta);

	return 0;
}

HRESULT CDynamicCamera::AddComponent(void)
{
	Engine::CComponent* pComponent = nullptr;

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Transform", pComponent);

	return S_OK;
}

HRESULT CDynamicCamera::Key_Input(const _float& fTimeDelta)
{
	m_pTransCom->Get_Info(Engine::INFO_LOOK, &m_vDir);

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_pTransCom->m_vInfo[Engine::INFO_POS] += m_vDir * 10.f * fTimeDelta;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_pTransCom->m_vInfo[Engine::INFO_POS] += m_vDir * -10.f * fTimeDelta;
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_pTransCom->m_vInfo[Engine::INFO_POS] +=
			m_pTransCom->m_vInfo[Engine::INFO_RIGHT] * -10.f * fTimeDelta;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_pTransCom->m_vInfo[Engine::INFO_POS] +=
			m_pTransCom->m_vInfo[Engine::INFO_RIGHT] * 10.f * fTimeDelta;
	}
	return S_OK;
}

HRESULT CDynamicCamera::Make_TransformCom(void)
{
	_vec3 vPos = m_pTransCom->m_vInfo[Engine::INFO_POS];
	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	m_vCurMouse = _vec3(float(pt.x), float(pt.y), 0.f);

	_vec3 vDir = m_vCurMouse - m_vPreMouse;
//	D3DXVec3Normalize(&vDir, &vDir);
	_matrix matRotX, matRotY, matScale, matTrans;

	D3DXMatrixScaling(&matScale, m_pTransCom->m_vScale.x, m_pTransCom->m_vScale.y, m_pTransCom->m_vScale.z);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);

	if (m_pTransCom->m_vInfo[Engine::INFO_UP].y <= 0.f)
	{
		vDir.x *= -1.f;
	}

	D3DXMatrixRotationX(&matRotX, D3DXToRadian(vDir.y));
	D3DXMatrixRotationY(&matRotY, D3DXToRadian(vDir.x));

	m_pTransCom->m_matWorld = matScale * matRotX * matRotY * matTrans;

	_vec3 vUp = m_pTransCom->m_matWorld.m[1];

	MakeViewMatrix(&(vPos), &(vPos + m_pTransCom->m_matWorld.m[2]), &vUp);

	m_pTransCom->m_vInfo[Engine::INFO_RIGHT] = m_pTransCom->m_matWorld.m[0];
	m_pTransCom->m_vInfo[Engine::INFO_UP] = m_pTransCom->m_matWorld.m[1];
	m_pTransCom->m_vInfo[Engine::INFO_LOOK] = m_pTransCom->m_matWorld.m[2];

	return S_OK;
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDynamicCamera* pInstance = new CDynamicCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		ERR_BOX("DynamicCamera Create Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->MakeProjectionMatrix(
		D3DXToRadian(60.f), _float(WINCX) / WINCY,
		0.1f, 1000.f);

	return pInstance;
}
