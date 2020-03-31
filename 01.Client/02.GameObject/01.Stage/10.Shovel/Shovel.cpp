#include "stdafx.h"
#include "Shovel.h"
#include "Shovel_Bullet.h"
#include "Export_Function.h"

CShovel::CShovel(LPDIRECT3DDEVICE9 pGarphicDev)
	: CGameObject(pGarphicDev)
	, m_pParentBoneMatrix(nullptr)
	, m_pParentWorldMatrix(nullptr)
{
}

CShovel::~CShovel(void)
{
}

void CShovel::Set_Throw(const _float& fRotY)
{
	if (!m_bIsThrow)
	{
		m_bIsThrow = true;
		Engine::Get_Layer(L"GameLogic")->Add_GameObject(L"Bullet", CShovel_Bullet::Create(m_pGraphicDev, m_pTargetTransCom->m_vInfo[Engine::INFO_LOOK],
			m_pTargetTransCom->m_vInfo[Engine::INFO_POS], fRotY));
	}
	// �� X ���Ϳ���
}

HRESULT CShovel::Ready_Object(Engine::CDynamicMesh* pTargetMeshCom, Engine::CTransform* pTargetTransCom, const _tchar* pMeshName)
{
	FAILED_CHECK_RETURN(Add_Component(pMeshName), E_FAIL);

	if (nullptr == pTargetMeshCom || nullptr == pTargetTransCom)
		return E_FAIL;
	m_pTransCom->Rotation(Engine::ROT_Y, D3DXToRadian(90.f));

	const Engine::D3DXFRAME_DERIVED* pFrame = pTargetMeshCom->Get_FrameByName("Bip01_R_Hand");
	m_pParentBoneMatrix = &pFrame->CombinedTransformMatrix; // �ش� ��ġ ���� ���.

	m_pParentWorldMatrix = pTargetTransCom->Get_WorldMatrix();

	m_pTargetMeshCom = pTargetMeshCom;
	m_pTargetTransCom = pTargetTransCom;

	return S_OK;
}

_int CShovel::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);

	if (m_bIsThrow)
	{
		m_fThrowTimer -= fTimeDelta;
		if (m_fThrowTimer <= 0.f)			// �ð������� ������ ���ƿͶ�.
		{
			m_bIsThrow = false;
			m_fThrowTimer = 3.f;
		}
	}

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	return 0;
}

void CShovel::Render_Object(void)
{
	if (m_bIsThrow)
		return;

	_matrix matBone = {};
	D3DXMatrixIdentity(&matBone);

	const Engine::D3DXFRAME_DERIVED* pRootBone = m_pTargetMeshCom->Get_FrameByName("Bip01");
	_matrix matRootBone = pRootBone->CombinedTransformMatrix;

	_matrix matRootWorld;
	D3DXMatrixIdentity(&matRootWorld);

	memcpy(matRootWorld.m[3], matRootBone.m[3], sizeof(_vec3));
	matRootWorld._42 = 0.f;
	D3DXMatrixInverse(&matRootWorld, nullptr, &matRootWorld);

	matBone = *m_pParentBoneMatrix;
	matBone._41 -= 50.f;
	matBone._43 += 200.f;

	m_pTransCom->Set_ParentMatrix(matBone * matRootWorld * (*m_pParentWorldMatrix)); // ������ �θ������ ���ع�����.

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
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

HRESULT CShovel::Add_Component(const _tchar* pMeshName)
{
	Engine::CComponent* pComponent = nullptr;


	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRenderCom = dynamic_cast<Engine::CRenderer*>(Engine::Clone(L"Renderer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone_Resources(RESOURCE_STAGE, pMeshName));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CShovel::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix		matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &m_pTransCom->m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	// ���� ���߿� �߰��ϴ� �������� ���°� ���� ���� �ֽ��ϴ�.

	//const D3DLIGHT9*	pLightInfo = Engine::Get_LightInfo();
	//NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	//pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));

	//pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLightInfo->Diffuse);
	//pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLightInfo->Ambient);
	//pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLightInfo->Specular);

	//D3DMATERIAL9		tMtrlInfo;
	//ZeroMemory(&tMtrlInfo, sizeof(D3DMATERIAL9));

	//tMtrlInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	//tMtrlInfo.Power = 20.f;

	//pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrlInfo.Diffuse);
	//pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrlInfo.Ambient);
	//pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrlInfo.Specular);
	//pEffect->SetFloat("g_fPower", tMtrlInfo.Power);

	//D3DXMatrixInverse(&matView, NULL, &matView);
	//pEffect->SetVector("g_vCamPos", (_vec4*)&matView._41);


	Safe_Release(pEffect);

	return S_OK;
}

CShovel * CShovel::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CDynamicMesh* pTargetMeshCom, Engine::CTransform* pTargetTransCom, const _tchar* pMeshName)
{
	CShovel* pInstance = new CShovel(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pTargetMeshCom, pTargetTransCom, pMeshName)))
	{
		ERR_BOX("Shovel Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CShovel::Free(void)
{
	Engine::CGameObject::Free();
}
