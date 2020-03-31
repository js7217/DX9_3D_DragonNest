#include "stdafx.h"
#include "StaticCamera.h"

#include "Export_Function.h"

#include "Player.h"
CStaticCamera::CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CNaviMesh* pNaviMesh)
	: CCamera(pGraphicDev)
	, m_pNaviMeshCom(pNaviMesh)
	, m_pCamTarget(nullptr), m_pTarget(nullptr)
	, m_fCamAngle(0.f), m_fCamSpeed(0.f), m_fCamAngleY(0.f), m_fTargetDist(0.f)
	, m_bIsClick(false), m_bMouseFix(false)
{
}

CStaticCamera::~CStaticCamera(void)
{

}

void CStaticCamera::Set_Target(CPlayer * pTarget)
{
	m_pTarget = pTarget;
	m_pCamTarget = m_pTarget->Get_TransformCom();
}

HRESULT CStaticCamera::Ready_Object(void)
{
	m_fTargetDist = 1.5f;
	m_fCamAngle = 45.f;
	m_fCamSpeed = 10.f;

//	Make_TransformCom();

	//_int iIndex = 0;
	//m_pNaviMeshCom->Get_PickCell(m_pCamTarget->m_vInfo[Engine::INFO_POS], &iIndex);
	m_pNaviMeshCom->SetCurrentIndex(0);

	return S_OK;
}

_int CStaticCamera::Update_Object(const _float& fTimeDelta)
{
	//FAILED_CHECK_RETURN(Key_Input(fTimeDelta), -1);
	//FAILED_CHECK_RETURN(Make_TransformCom(), -1);
	Engine::CGameObject::Update_Object(fTimeDelta);

	if (false == m_bMouseFix)
	{
		FAILED_CHECK_RETURN(Key_Input(fTimeDelta), -1);
		FAILED_CHECK_RETURN(Make_TransformCom(), -1);
	}
	else
		Rotate_TargetMouse();

	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };
	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);

	Change_CamMode(); // Shift Ű�� ������ ī�޶��� ��带 �ٲ۴�.


	return 0;
}


HRESULT CStaticCamera::Key_Input(const _float& fTimeDelta)
{
	if (GetAsyncKeyState('Q') & 0x8000)
		m_fTargetDist -= m_fCamSpeed * fTimeDelta;
	if (GetAsyncKeyState('E') & 0x8000)
		m_fTargetDist += m_fCamSpeed * fTimeDelta;

	// Mouse Move
	_long dwMouseMove = 0;
	if (dwMouseMove = Engine::Get_DIMouseMove(Engine::CInputDev::DIMS_Y)) // ���콺 Y�� Move �� �÷��̾��� ��ġ�� �ٲ� �ȵȴ�.
	{
		m_fCamAngle += dwMouseMove * 0.1f;
	}
	if (dwMouseMove = Engine::Get_DIMouseMove(Engine::CInputDev::DIMS_X))
	{
		if(CPlayer::BASIC == m_pTarget->Get_AttMode())
			m_pCamTarget->Rotation(Engine::ROT_Y, D3DXToRadian(dwMouseMove * 0.1f));
	}


	return S_OK;
}

HRESULT CStaticCamera::Make_TransformCom(void)
{
	m_vEye = m_pCamTarget->m_vInfo[Engine::INFO_LOOK] * -1.f;
	D3DXVec3Normalize(&m_vEye, &m_vEye);
	m_vEye *= m_fTargetDist;
	// ���̴� ī�޶��� ������ ���� ������ ��.
	D3DXVECTOR3 vRight = {};
	memcpy(&vRight, m_pCamTarget->m_matWorld.m[0], sizeof(D3DXVECTOR3));

	// right ���� �������� �� ȸ��.
	D3DXMATRIX matRotAxis;

	// D3DXMatrixRotationAxis: ������ �� ȸ��
	D3DXMatrixRotationAxis(&matRotAxis, &vRight, D3DXToRadian(m_fCamAngle));
	D3DXVec3TransformNormal(&m_vEye, &m_vEye, &matRotAxis);

	// ī�޶��� ��ġ
	D3DXVec3Normalize(&vRight, &vRight);
	vRight.y += 4.f;
	m_vEye += m_pCamTarget->m_vInfo[Engine::INFO_POS];			// �ش� Eye�� ������ �׺�޽����� �񱳰� �����Ѵ�.
	m_vAt = m_pCamTarget->m_vInfo[Engine::INFO_POS] + vRight * 0.1f;

	MakeViewMatrix(&m_vEye, &m_vAt, &_vec3(0.f, 1.f, 0.f));
	return S_OK;
}

void CStaticCamera::Fix_Mouse(void)
{
	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };
	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CStaticCamera::Rotate_TargetMouse(void)
{
	// Mouse Move
	_long dwMouseMove = 0;
	if (dwMouseMove = Engine::Get_DIMouseMove(Engine::CInputDev::DIMS_Y)) // ���콺 Y�� Move �� �÷��̾��� ��ġ�� �ٲ� �ȵȴ�.
	{
		m_fCamAngle += dwMouseMove * 0.1f;
	}
	if (dwMouseMove = Engine::Get_DIMouseMove(Engine::CInputDev::DIMS_X))
	{
		m_fCamAngleY += dwMouseMove * 0.1f;
	}

	if (m_fCamAngle >= 90.f) // ���̻� ���� ������ �ȵǰ� �������ش�.
		m_fCamAngle = 89.f;

	m_vEye = m_pCamTarget->m_vInfo[Engine::INFO_LOOK] * -1.f;
	D3DXVec3Normalize(&m_vEye, &m_vEye);
	m_vEye *= m_fTargetDist;

	// ���̴� ī�޶��� ������ ���� ������ ��.
	D3DXVECTOR3 vRight = {};
	memcpy(&vRight, m_pCamTarget->m_matWorld.m[0], sizeof(D3DXVECTOR3));

	_vec3 vUp = {};
	memcpy(&vUp, m_pCamTarget->m_matWorld.m[1], sizeof(_vec3));

	// right ���� �������� �� ȸ��.
	D3DXMATRIX matRotAxis;

	// D3DXMatrixRotationAxis: ������ �� ȸ��
	D3DXMatrixRotationAxis(&matRotAxis, &vRight, D3DXToRadian(m_fCamAngle));
	D3DXVec3TransformNormal(&m_vEye, &m_vEye, &matRotAxis);

	D3DXMatrixIdentity(&matRotAxis);
	D3DXMatrixRotationAxis(&matRotAxis, &vUp, D3DXToRadian(m_fCamAngleY));
	D3DXVec3TransformNormal(&m_vEye, &m_vEye, &matRotAxis);

	//ī�޶��� ��ġ
	m_vEye += m_pCamTarget->m_vInfo[Engine::INFO_POS];
	m_vAt = m_pCamTarget->m_vInfo[Engine::INFO_POS];

	MakeViewMatrix(&m_vEye, &m_vAt, &_vec3(0.f, 1.f, 0.f));	
}

void CStaticCamera::Change_CamMode()
{
	if (Engine::Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{
		if (true == m_bIsClick)
			return;

		m_bIsClick = true;

		if (true == m_bMouseFix)
			m_bMouseFix = false;
		else
			m_bMouseFix = true;

		m_fCamAngle = 0.f;
		m_fCamAngleY = 0.f;
	}
	else
		m_bIsClick = false;
}

CStaticCamera* CStaticCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, CPlayer* pTarget, Engine::CNaviMesh* pNaviMesh)
{
	CStaticCamera* pInstance = new CStaticCamera(pGraphicDev, pNaviMesh);

	if (FAILED(pInstance->Ready_Object()))
	{
		ERR_BOX("StaticCamera Create Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->Set_Target(pTarget);
	pInstance->MakeProjectionMatrix(
		D3DXToRadian(60.f), _float(WINCX) / WINCY,
		0.1f, 1000.f);

	return pInstance;
}
