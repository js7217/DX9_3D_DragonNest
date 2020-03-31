#include "stdafx.h"
#include "Door.h"

#include "Export_Function.h"
#include "Player.h"
#include "Stone.h"
CDoor::CDoor(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_wstrName(L"Door")
{

}

CDoor::~CDoor(void)
{

}

void CDoor::Set_Pos(const _vec3 & vPos)
{
	m_pTransCom->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CDoor::Set_Size(const _vec3 & vSize)
{
	m_pTransCom->Set_Scale(vSize.x, vSize.y, vSize.z);
}

void CDoor::Set_Rotate(const _vec3 & vRotate)
{
	m_pTransCom->m_vAngle = vRotate;
}

Engine::CTransform * CDoor::Get_TransformCom()
{
	return m_pTransCom;
}


HRESULT CDoor::Ready_Object()
{
	Engine::CGameObject::Ready_Object();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_iHp = 10;

	m_pMeshCom->Set_AnimationSet(3);
	m_eCurState = CGameObject::DAMAGE;
	return S_OK;
}

_int CDoor::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);
	m_fTimeDelta = fTimeDelta;
	MotionChange();
	MotionEvent();

	if (m_bIsCreateStone)
		CreateStone(fTimeDelta);

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	return 0;
}

void CDoor::Render_Object(void)
{
	m_pMeshCom->Play_AnimationSet(m_fTimeDelta);

	LPD3DXEFFECT		pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );
	
	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	m_pMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

}

HRESULT CDoor::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pTransCom = Engine::CTransform::Create(_vec3(1.f, 0.f, 0.f));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, L"Mesh_Prison_Door"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CDoor::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix		matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &m_pTransCom->m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	Safe_Release(pEffect);

	return S_OK;
}


void CDoor::MotionChange()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Engine::CGameObject::STAND:
			m_pMeshCom->Set_AnimationSet(0);
			break;
		case Engine::CGameObject::WALK:
			{
				Engine::CComponent* pComponent = Engine::Get_Component(L"GameLogic", L"Player", L"Com_Navi", Engine::ID_STATIC);
				dynamic_cast<Engine::CNaviMesh*>(pComponent)->Change_CellOption(Engine::CCell::NORMAL, Engine::CCell::TRIGGER, m_iGroup);
			}
			m_pMeshCom->Set_AnimationSet(1);
			break;
		case Engine::CGameObject::ATTACK:
			m_pMeshCom->Set_AnimationSet(2);
			break;
		case Engine::CGameObject::DAMAGE:
			m_pMeshCom->Set_AnimationSet(3);
			break;
		default:
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CDoor::MotionEvent()
{
	switch (m_eCurState)
	{
	case Engine::CGameObject::STAND:	// 닫히는 모습
		if (m_pMeshCom->Is_AnimationSetEnd())
			m_eCurState = DAMAGE;
		break;
	case Engine::CGameObject::WALK:		// 열리는 모습
		if (m_pMeshCom->Is_AnimationSetEnd(0.5))
			m_eCurState = ATTACK;
		break;
	case Engine::CGameObject::ATTACK:	// 열림
		break;
	case Engine::CGameObject::DAMAGE:	// 닫힘
		if (1 == Engine::Get_Layer(L"GameLogic")->Count_Group(m_iGroup))
		{
			_uint iNextGroup = m_iGroup + 1;
			Engine::Get_Layer(L"GameLogic")->Open_Door(iNextGroup);
			m_eCurState = WALK;
			if(0 == m_iGroup)
				m_bIsCreateStone = true;	// 돌을 생성할 것이다.
		}
		break;
	default:
		break;
	}
}

void CDoor::CreateStone(const _float& fTimeDelta)
{
	if (1 != Engine::Get_Layer(L"GameLogic")->Get_GroupCount())
		m_bIsCreateStone = false;

	if (m_fStoneCreateTimer <= 0.f)
	{
		Engine::Get_Layer(L"GameLogic")->Reset_HitObject(L"Player");
		Engine::CGameObject* pStone = CStone::Create(m_pGraphicDev, _vec3(24.9338f, 0.f, 5.83f), _vec3(3e-05f, 3e-05f, 3e-05f), _vec3(0.f, D3DXToRadian(90.f), 0.f));
		NULL_CHECK_RETURN(pStone, );
		Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"Stone", pStone);

		m_fStoneCreateTimer = 1.5f;
	}

	m_fStoneCreateTimer -= fTimeDelta;
}

CDoor * CDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vSize, const _vec3& vRotate, const _uint& iGroup)
{
	CDoor* pInstance = new CDoor(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		ERR_BOX("Door Created Failed");
		Engine::Safe_Release(pInstance);
	}

	pInstance->Set_Pos(vPos);
	pInstance->Set_Size(vSize);
	pInstance->Set_Rotate(vRotate);
	pInstance->Set_Group(iGroup);

	return pInstance;
}

void CDoor::Free(void)
{
	Engine::CGameObject::Free();
}