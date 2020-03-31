#include "stdafx.h"
#include "Stage.h"
#include "BossStage.h"
#include "Export_Function.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{

}

CStage::~CStage(void)
{
}

HRESULT CStage::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Engine::CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Resources(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_ProtoType(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Light(), E_FAIL);

//	CSoundMgr::GetInstance()->PlayBGM(L"BGM.mp3");

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = CScene::Update_Scene(fTimeDelta);

	//if (GetAsyncKeyState('3') & 0x8000)
	//{
	//	Engine::CScene* pScene = NULL;

	//	pScene = CBossStage::Create(m_pGraphicDev);
	//	NULL_CHECK_RETURN(pScene, -1);

	//	if (FAILED(Engine::SetUp_CurrentScene(pScene)))
	//		return -1;

	//	return iExit;

	//}
	return iExit;
}

void CStage::Render_Scene(void)
{
//	CScene::Render_Scene();
	if (GetAsyncKeyState('3') & 0x8000)
	{
		Engine::CScene* pScene = NULL;

		pScene = CBossStage::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pScene, );

		if (FAILED(Engine::SetUp_CurrentScene(pScene)))
			return;

		return;

	}
}

HRESULT CStage::Ready_Layer_GameLogic(const _tchar* pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	// Player, Monster, SkyBox Add
	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);
	

	Engine::CGameObject* pTerrain = nullptr;
	pTerrain = Load_Terrain(pLayer);

	//pGameObject = CTerrain::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);


	//pGameObject = CDynamicCamera::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);
	

	Load_Mesh(pLayer, pTerrain, nullptr); // Static, Dynamic, Navi Mesh Load
	m_mapLayer.emplace(pLayerTag, pLayer);

	pLayer->Open_Door(0);	// 0번 그룹은 만들어져라.
	return S_OK;
}

HRESULT CStage::Ready_Resources(void)
{
	return S_OK;
}

HRESULT CStage::Ready_Camera(void)
{
	// 임시코드
	_matrix		matView, matProj;

	D3DXMatrixLookAtLH(&matView,
		&_vec3(0.f, 0.f, -5.f),
		&_vec3(0.f, 0.f, 0.f),
		&_vec3(0.f, 1.f, 0.f));
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixPerspectiveFovLH(&matProj,
		D3DXToRadian(60.f),
		_float(WINCX) / WINCY,
		0.1f,
		500.f);

	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	return S_OK;
}

HRESULT CStage::Ready_Light(void)
{
	D3DLIGHT9 tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);			// 방향성 광원은 스펙큘러를 없앤 상태에서 한다.
	tLightInfo.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
	tLightInfo.Direction = _vec3(0.f, -0.05f, 5.f);

	if (FAILED(Engine::Add_Light(m_pGraphicDev, tLightInfo, 100)))
		return E_FAIL;

	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_POINT;
	tLightInfo.Position = _vec3(10.2f, 0.5f, 3.f);
	tLightInfo.Range = 1.f;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 0.3f, 0.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.05f, 0.05f, 0.05f, 1.f);

	if (FAILED(Engine::Add_Light(m_pGraphicDev, tLightInfo, 0)))
		return E_FAIL;

	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_POINT;
	tLightInfo.Position = _vec3(8.8f, 0.5f, 3.f);
	tLightInfo.Range = 1.f;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 0.3f, 0.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.05f, 0.05f, 0.05f, 1.f);

	if (FAILED(Engine::Add_Light(m_pGraphicDev, tLightInfo, 0)))
		return E_FAIL;
/*
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_POINT;
	tLightInfo.Position = _vec3(11.f, 0.5f, 5.8f);
	tLightInfo.Range = 1.f;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 0.3f, 0.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.05f, 0.05f, 0.05f, 1.f);

	if (FAILED(Engine::Add_Light(m_pGraphicDev, tLightInfo)))
		return E_FAIL;

	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_POINT;
	tLightInfo.Position = _vec3(15.f, 0.5f, 5.8f);
	tLightInfo.Range = 1.f;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 0.3f, 0.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.05f, 0.05f, 0.05f, 1.f);

	if (FAILED(Engine::Add_Light(m_pGraphicDev, tLightInfo)))
		return E_FAIL;

	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_POINT;
	tLightInfo.Position = _vec3(20.f, 0.5f, 5.8f);
	tLightInfo.Range = 1.f;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 0.3f, 0.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.05f, 0.05f, 0.05f, 1.f);

	if (FAILED(Engine::Add_Light(m_pGraphicDev, tLightInfo)))
		return E_FAIL;
*/
	return S_OK;
}

HRESULT CStage::Ready_ProtoType(void)
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pNaviMesh = Engine::CNaviMesh::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::Ready_Prototype(L"Proto_NaviMesh", pComponent);


	pComponent = Engine::COptimization::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::Ready_Prototype(L"Proto_Optimization", pComponent);

	return S_OK;
}

Engine::CGameObject* CStage::Load_Terrain(Engine::CLayer* pLayer)
{
	HANDLE hFile = CreateFile(L"../../Data/Terrain/Terrain_Prison.dat", GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"Terrain Load Failed", nullptr, MB_OK);
		return nullptr;
	}

	int iCntX = 0;
	int iCntZ = 0;
	int iItv = 0;
	int iDetail = 0;
	int iTexNum = 0;

	DWORD dwByte = 0;

	while (true)
	{
		ReadFile(hFile, &iCntX, sizeof(int), &dwByte, nullptr);
		ReadFile(hFile, &iCntZ, sizeof(int), &dwByte, nullptr);
		ReadFile(hFile, &iItv, sizeof(int), &dwByte, nullptr);
		ReadFile(hFile, &iDetail, sizeof(int), &dwByte, nullptr);
		ReadFile(hFile, &iTexNum, sizeof(int), &dwByte, nullptr);

		if (0 == dwByte)
			break;
	}

	CloseHandle(hFile);


	Engine::Delete_Resources(RESOURCE_STATIC, L"Buffer_TerrainTex"); // 해당 리소스 삭제

	FAILED_CHECK_RETURN(Engine::Ready_Buffers(m_pGraphicDev,
		RESOURCE_STATIC,
		L"Buffer_TerrainTex",
		Engine::BUFFER_TERRAINTEX, iCntX, iCntZ, iItv, iDetail), nullptr); // 옵션에 맞게 재생성


	Engine::CGameObject* pGameObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, nullptr);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), nullptr);

	dynamic_cast<CTerrain*>(pGameObject)->Set_TexNum(iTexNum);

	return pGameObject;
}

HRESULT CStage::Load_Mesh(Engine::CLayer * pLayer, Engine::CGameObject* pTerrain, CPlayer* pPlayer)
{
	Engine::CNaviMesh* pNaviMesh = nullptr;
	//dynamic_cast<CTerrain*>(pTerrain)->Get_NaviMesh(&pNaviMesh);			// Terrain의 NaviMesh 를 가져온다.
	//NULL_CHECK_RETURN(pNaviMesh, E_FAIL);
	HANDLE hFile = CreateFile(L"../../Data/Mesh/Prison_Light_Index.dat", GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"Mesh Load Failed", nullptr, MB_OK);
		return E_FAIL;
	}

	DWORD dwBytes = 0;
	_uint iSize = 0;

	////////////NaviMesh///////////////////
	ReadFile(hFile, &iSize, sizeof(_uint), &dwBytes, nullptr);

	while (iSize-- > 0)
	{
		_vec3 vPos[3] = {}; // Cell의 위치값.
		ZeroMemory(&vPos, sizeof(_vec3) * 3);
		Engine::CCell::CELL_OPTION eOption = Engine::CCell::NORMAL; // 초기화.
		_uint iGroup = 100;
		ReadFile(hFile, &eOption, sizeof(Engine::CCell::CELL_OPTION), &dwBytes, nullptr);
		ReadFile(hFile, vPos, sizeof(_vec3) * 3, &dwBytes, nullptr);
		ReadFile(hFile, &iGroup, sizeof(_uint), &dwBytes, nullptr);

		for (_uint i = 0; i < 3; ++i)
		{
			m_pNaviMesh->Set_CellPos(vPos[i], eOption, iGroup);
			//dynamic_cast<CTerrain*>(pTerrain)->Create_Cell(vPos[i], eOption);
		}
	}
	m_pNaviMesh->Link_Cell();
	//dynamic_cast<CTerrain*>(pTerrain)->Link_Cell();

	//////////////Static Mesh///////////////////////////
	ReadFile(hFile, &iSize, sizeof(_uint), &dwBytes, nullptr); //먼저 사이즈 부터 받아오게 함.

	while (iSize-- > 0)
	{
		TCHAR szTag[256] = L"";
		_ulong fLength = 0;
		_vec3 vPos = { 0.f, 0.f, 0.f };
		_vec3 vSize = { 0.f, 0.f, 0.f };
		_vec3 vRotate = { 0.f, 0.f, 0.f };
		_uint iGroup = 0;

		// Static
		ReadFile(hFile, &fLength, sizeof(_ulong), &dwBytes, nullptr);
		ReadFile(hFile, szTag, sizeof(TCHAR) * fLength, &dwBytes, nullptr);

		ReadFile(hFile, &vPos, sizeof(_vec3), &dwBytes, nullptr);
		ReadFile(hFile, &vSize, sizeof(_vec3), &dwBytes, nullptr);
		ReadFile(hFile, &vRotate, sizeof(_vec3), &dwBytes, nullptr);
		ReadFile(hFile, &iGroup, sizeof(_uint), &dwBytes, nullptr);

		if (0 == dwBytes)
			break;

		Engine::CGameObject* pGameObject = nullptr;

		if (!lstrcmp(szTag, L"Mesh_Prison_Burner"))
		{
			D3DLIGHT9 tLightInfo;
			ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

			tLightInfo.Type = D3DLIGHT_POINT;
			tLightInfo.Position = vPos;
			tLightInfo.Range = 2.f;
			tLightInfo.Diffuse = D3DXCOLOR(1.f, 0.3f, 0.f, 1.f);
			tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			tLightInfo.Specular = D3DXCOLOR(0.05f, 0.05f, 0.05f, 1.f);

			if (FAILED(Engine::Add_Light(m_pGraphicDev, tLightInfo, iGroup)))
				return E_FAIL;
		}
		else
		{
			pGameObject = CStaticObject::Create(m_pGraphicDev, szTag, vPos, vSize, vRotate);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject(dynamic_cast<CStaticObject*>(pGameObject)->Get_MeshName().c_str(), pGameObject), E_FAIL);
		}
	
	}


	pPlayer = CPlayer::Create(m_pGraphicDev, m_pNaviMesh);
	NULL_CHECK_RETURN(pPlayer, E_FAIL);

	Engine::CGameObject* pGameObject = CStaticCamera::Create(m_pGraphicDev, dynamic_cast<CPlayer*>(pPlayer), m_pNaviMesh);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"StaticCamera", pGameObject), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pPlayer), E_FAIL);

	// Dynamic Mesh : 이름따라 나뉘어서 생각해야함.
	ReadFile(hFile, &iSize, sizeof(_uint), &dwBytes, nullptr); //먼저 사이즈 부터 받아오게 함.

	while (iSize-- > 0)
	{
		TCHAR szTag[256] = L"";
		_ulong fLength = 0;
		_vec3 vPos = { 0.f, 0.f, 0.f };
		_vec3 vSize = { 0.f, 0.f, 0.f };
		_vec3 vRotate = { 0.f, 0.f, 0.f };
		_uint iGroup = 0;
		// Static
		ReadFile(hFile, &fLength, sizeof(_ulong), &dwBytes, nullptr);
		ReadFile(hFile, szTag, sizeof(TCHAR) * fLength, &dwBytes, nullptr);

		ReadFile(hFile, &vPos, sizeof(_vec3), &dwBytes, nullptr);
		ReadFile(hFile, &vSize, sizeof(_vec3), &dwBytes, nullptr);
		ReadFile(hFile, &vRotate, sizeof(_vec3), &dwBytes, nullptr);
		ReadFile(hFile, &iGroup, sizeof(_uint), &dwBytes, nullptr);

		if (0 == dwBytes)
			break;
		Engine::CGameObject* pGameObject = nullptr;
		 // 이름으로 먼저 구분하여 로드하기?????
		if (!lstrcmp(szTag, L"Mesh_Wing"))
		{
			pGameObject = CWing::Create(m_pGraphicDev, vPos, vSize, vRotate);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject
			(L"Wing", pGameObject), E_FAIL);
		}
		else if (!lstrcmp(szTag, L"Mesh_TinyDragon"))
		{
			pGameObject = CTinyDragon::Create(m_pGraphicDev, m_pNaviMesh, pPlayer, vPos, vSize, vRotate, iGroup);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject
			(L"TinyDragon", pGameObject), E_FAIL);
		}
		else if (!lstrcmp(szTag, L"Mesh_Goblin"))
		{
			CGoblin* pGoblin = nullptr;
			pGameObject = pGoblin = CGoblin::Create(m_pGraphicDev, m_pNaviMesh, pPlayer, vPos, vSize, vRotate, iGroup);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject
			(L"TinyDragon", pGameObject), E_FAIL);

			pGameObject = CSword::Create(m_pGraphicDev, pGoblin->Get_MeshCom(), pGoblin->Get_TransformCom(), L"Mesh_Goblin_Weapon");
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			pGoblin->Set_Sword(dynamic_cast<CSword*>(pGameObject));
		}
		else if (!lstrcmp(szTag, L"Mesh_Spartan"))
		{
			CSpartan* pGoblin = nullptr;

			pGameObject = pGoblin = CSpartan::Create(m_pGraphicDev, m_pNaviMesh, pPlayer, vPos, vSize, vRotate, iGroup);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject
			(L"TinyDragon", pGameObject), E_FAIL);

			pGameObject = CShovel::Create(m_pGraphicDev, pGoblin->Get_MeshCom(), pGoblin->Get_TransformCom(), L"Mesh_Spartan_Weapon");
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			pGoblin->Set_Sword(dynamic_cast<CShovel*>(pGameObject));
		}
		else if (!lstrcmp(szTag, L"Mesh_Orge"))
		{
			COrge* pOrge = nullptr;

			pGameObject = pOrge = COrge::Create(m_pGraphicDev, m_pNaviMesh, pPlayer, vPos, vSize, vRotate, iGroup);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject
			(L"TinyDragon", pGameObject), E_FAIL);

			pGameObject = CSword::Create(m_pGraphicDev, pOrge->Get_MeshCom(), pOrge->Get_TransformCom(), L"Mesh_Orge_Weapon");
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			pOrge->Set_Sword(dynamic_cast<CSword*>(pGameObject));
		}
		else if (!lstrcmp(szTag, L"Mesh_Prison_Door"))
		{
			pGameObject = CDoor::Create(m_pGraphicDev, vPos, vSize, vRotate, iGroup);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Door", pGameObject), E_FAIL);
		}
		else if (!lstrcmp(szTag, L"Mesh_Stone"))
		{
			//pGameObject = CScythe::Create(m_pGraphicDev, vPos, vSize, vRotate);
			//NULL_CHECK_RETURN(pGameObject, E_FAIL);
			//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Scythe", pGameObject), E_FAIL);
		}

		else if (!lstrcmp(szTag, L"Mesh_scythe"))
		{
			pGameObject = CScythe::Create(m_pGraphicDev, vPos, vSize, vRotate);
			pGameObject->Set_Group(iGroup);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Scythe", pGameObject), E_FAIL);
		}
		else if (!lstrcmp(szTag, L"Mesh_Trap1"))
		{
			pGameObject = CThorn::Create(m_pGraphicDev, vPos, vSize, vRotate);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Thorn", pGameObject), E_FAIL);
		}
		else
		{
			pGameObject = CStaticObject::Create(m_pGraphicDev, szTag, vPos, vSize, vRotate);

			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject(
				dynamic_cast<CStaticObject*>(pGameObject)->Get_MeshName().c_str(),
				pGameObject), E_FAIL);
		}
	}

	CloseHandle(hFile);

	// 테스트용
	// MainBar
	pGameObject = CShortcuts::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MainBar", pGameObject), E_FAIL);
	// FaceUI
	pGameObject = CFaceUI::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"FaceUI", pGameObject), E_FAIL);

	//Bar_Back
	for (_uint i = 0; i < 2; ++i)
	{
		pGameObject = CBar_Back::Create(m_pGraphicDev, 179.f, 20.f * (i + 1));
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Bar_Back", pGameObject), E_FAIL);
	}

	// HpBar
	pGameObject = CHpBar::Create(m_pGraphicDev, dynamic_cast<CPlayer*>(pPlayer));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"HpBar", pGameObject), E_FAIL);

	// MpBar
	pGameObject = CMpBar::Create(m_pGraphicDev, dynamic_cast<CPlayer*>(pPlayer));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MpBar", pGameObject), E_FAIL);

	// MouseUI
	pGameObject = CMouseUI::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MouseUI", pGameObject), E_FAIL);

	return S_OK;
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage* pInstance = new CStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		ERR_BOX("Stage Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage::Free(void)
{
	CSoundMgr::GetInstance()->StopSound(CSoundMgr::BGM);
//	Engine::Clear_Renderer();
	Engine::Safe_Release(m_pLight);
	CScene::Free();
}
