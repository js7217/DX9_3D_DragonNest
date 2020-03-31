#include "stdafx.h"
#include "Sword.h"

#include "Export_Function.h"

CSword::CSword(LPDIRECT3DDEVICE9 pGarphicDev)
	: CGameObject(pGarphicDev)
	, m_pParentBoneMatrix(nullptr)
	, m_pParentWorldMatrix(nullptr)
{
}

CSword::~CSword(void)
{
}

HRESULT CSword::Ready_Object(Engine::CDynamicMesh* pTargetMeshCom, Engine::CTransform* pTargetTransCom, const _tchar* pMeshName)
{
	FAILED_CHECK_RETURN(Add_Component(pMeshName), E_FAIL);

	if (nullptr == pTargetMeshCom || nullptr == pTargetTransCom)
		return E_FAIL;
	m_pTransCom->Rotation(Engine::ROT_Z, D3DXToRadian(-80.f));
	m_pTransCom->Rotation(Engine::ROT_Y, D3DXToRadian(-90.f));

	const Engine::D3DXFRAME_DERIVED* pFrame = pTargetMeshCom->Get_FrameByName("Bip01_R_Hand");
	m_pParentBoneMatrix = &pFrame->CombinedTransformMatrix; // 해당 위치 뼈의 행렬.

	m_pParentWorldMatrix = pTargetTransCom->Get_WorldMatrix();

	m_pTargetMeshCom = pTargetMeshCom;
	return S_OK;
}

_int CSword::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pRenderCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	return 0;
}

void CSword::Render_Object(void)
{
	const Engine::D3DXFRAME_DERIVED* pRootBone = m_pTargetMeshCom->Get_FrameByName("Bip01");
	_matrix matRootBone = pRootBone->CombinedTransformMatrix;

	_matrix matRootWorld;
	D3DXMatrixIdentity(&matRootWorld);

	memcpy(matRootWorld.m[3], matRootBone.m[3], sizeof(_vec3));
	matRootWorld._42 = 0.f;
	D3DXMatrixInverse(&matRootWorld, nullptr, &matRootWorld);

	_matrix matBone = *m_pParentBoneMatrix;
	matBone._41 -= 50.f;
	matBone._43 += 200.f;
	m_pTransCom->Set_ParentMatrix(matBone * matRootWorld * (*m_pParentWorldMatrix)); // 무기의 부모행렬을 곱해버린다.
																	  // 순서 주의. 본 행렬을 먼저 해줘야함.
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

HRESULT CSword::Add_Component(const _tchar* pMeshName)
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

HRESULT CSword::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix		matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &m_pTransCom->m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	// 빛을 나중에 추가하는 방향으로 가는게 좋을 수도 있습니다.

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

CSword * CSword::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CDynamicMesh* pTargetMeshCom, Engine::CTransform* pTargetTransCom, const _tchar* pMeshName)
{
	CSword* pInstance = new CSword(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pTargetMeshCom, pTargetTransCom, pMeshName)))
	{
		ERR_BOX("Sword Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CSword::Free(void)
{
	Engine::CGameObject::Free();
}
