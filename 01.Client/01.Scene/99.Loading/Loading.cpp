#include "stdafx.h"
#include "Loading.h"

#include "Export_Function.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
{
	m_pGraphicDev->AddRef();
	ZeroMemory(m_szLoading, sizeof(_tchar) * 128);
}

CLoading::~CLoading(void)
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eLoadingID)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

	m_eLoadingID = eLoadingID;

	return S_OK;
}

_uint CLoading::Loading_ForStage(void)
{
	lstrcpy(m_szLoading, L"Buffer Loading...........");

	FAILED_CHECK_RETURN(Engine::Ready_Buffers(m_pGraphicDev,
		RESOURCE_STATIC,
		L"Buffer_CubeTex",
		Engine::BUFFER_CUBETEX),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffers(m_pGraphicDev,
		RESOURCE_STATIC,
		L"Buffer_TerrainTex",
		Engine::BUFFER_TERRAINTEX, 129, 129, 1),
		E_FAIL);

	lstrcpy(m_szLoading, L"Texture Loading.........");

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Terrain",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/Terrain/Tile0.jpg",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_SkyBox",
		Engine::TEX_CUBE,
		L"../Bin/Resources/Texture/SkyBox/Burger%d.dds",
		4),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_dust",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/dust/dust%d.dds",
		4),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Player",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/Player/Ma.jpg",
		1),
		E_FAIL);


	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Explosion",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/Explosion/Explosion%d.png",
		90),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Filter",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/Terrain/Filter.bmp",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Brush",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/Terrain/Aura0.tga",
		1),
		E_FAIL);

	// uit_mainbar_re_slot
	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_MainBar",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/UI/uit_mainbar_re_slot.png",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_FaceUI",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/UI/FaceUI.png",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Bar_Back",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/UI/Bar_Back.png",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Bar_HP",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/UI/Bar_HP2.png",
		1),
		E_FAIL);

	//Texture_Bar_MP
	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Bar_MP",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/UI/Bar_MP2.png",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Dust_Left1",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/dust/dust2.png",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Dust_Right1",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/dust/dust3.png",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Dust_Rolling",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/dust/rush_dust2.dds",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Dust_Jump",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/dust/rush_dust.dds",
		1),
		E_FAIL);

	// dust01
	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Dust_Walk",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/dust/dust01.png",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Mouse",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/UI/MouseUI.png",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Hit",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/Hit/Hit.dds",
		1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Textures(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Texture_Player_Hit",
		Engine::TEX_NORMAL,
		L"../Bin/Resources/Texture/Hit/Player_Hit.dds",
		1),
		E_FAIL);

	lstrcpy(m_szLoading, L"Mesh Loading.........");

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Bottom",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Bottom/",
		L"Bottom.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Door_Close",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Door_Close/",
		L"Door_Close.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Door_Open",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Door_Open/",
		L"Door_Open.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Floor",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Floor/",
		L"Floor.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_path0",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/path0/",
		L"path0.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_path1",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/path1/",
		L"path1.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Pillar",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Pillar/",
		L"Pillar.X"), E_FAIL);


	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Room",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Room/",
		L"Room.X"), E_FAIL);


	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Path",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Stage/",
		L"Prison_Path.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_BossRoom",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/BossRoom/",
		L"Prison_BossRoom.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Goblin_Weapon",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Goblin_Weapon/",
		L"Goblin_Weapon.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Orge_Weapon",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Orge_Weapon/",
		L"Orge_Weapon.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Spartan_Weapon",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Goblin_Shovel/",
		L"Spartan_Weapon.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Goblin_Stone",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Goblin_Stone/",
		L"Goblin_Stone.X"), E_FAIL);

	//FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
	//	RESOURCE_STAGE,
	//	L"Mesh_Prison_Door",
	//	Engine::TYPE_STATIC,
	//	L"../Bin/Resources/Mesh/StaticMesh/Prison_Door/",
	//	L"Prison_Door.X"), E_FAIL);		// ´ÙÀÌ³ª¹ÍÀ¸·Î »¬°Í.

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_BlackDragon",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/BlackDragon/",
		L"BlackDragon.X"), E_FAIL);		// ´ÙÀÌ³ª¹ÍÀ¸·Î »¬°Í.


	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Path1",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Path1/",
		L"Prison_Path1.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Path2",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Path2/",
		L"Prison_Path2.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Path3",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Path3/",
		L"Prison_Path3.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Path4",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Path4/",
		L"Prison_Path4.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Path5",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Path5/",
		L"Prison_Path5.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Path6",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Path6/",
		L"Prison_Path6.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Path7",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Path7/",
		L"Prison_Path7.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Wall",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Wall/",
		L"Prison_Wall.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Brick1",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Brick/",
		L"Prison_Brick1.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Room",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Room/",
		L"Prison_Room.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Burner",
		Engine::TYPE_STATIC,
		L"../Bin/Resources/Mesh/StaticMesh/Prison_Burner/",
		L"Prison_Burner.X"), E_FAIL);
	//Dynamic
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Machina",
		Engine::TYPE_DYNAMIC,
		L"../Bin/Resources/Mesh/DynamicMesh/machina/",
		L"machina.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Wing",
		Engine::TYPE_DYNAMIC,
		L"../Bin/Resources/Mesh/DynamicMesh/Wing/",
		L"WingFull.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_TinyDragon",
		Engine::TYPE_DYNAMIC,
		L"../Bin/Resources/Mesh/DynamicMesh/TinyDragon/",
		L"Tiny.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Goblin",
		Engine::TYPE_DYNAMIC,
		L"../Bin/Resources/Mesh/DynamicMesh/Goblin/",
		L"Goblin.X"), E_FAIL);	

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Spartan",
		Engine::TYPE_DYNAMIC,
		L"../Bin/Resources/Mesh/DynamicMesh/Spartan/",
		L"Spartan.X"), E_FAIL);	

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Orge",
		Engine::TYPE_DYNAMIC,
		L"../Bin/Resources/Mesh/DynamicMesh/Orge/",
		L"Orge.X"), E_FAIL);	

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Prison_Door",
		Engine::TYPE_DYNAMIC,
		L"../Bin/Resources/Mesh/DynamicMesh/Prison_Door/",
		L"Prison_Door.X"), E_FAIL);		// ´ÙÀÌ³ª¹ÍÀ¸·Î »¬°Í.

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Stone",
		Engine::TYPE_DYNAMIC,
		L"../Bin/Resources/Mesh/DynamicMesh/Stone/",
		L"Stone.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_scythe",
		Engine::TYPE_DYNAMIC,
		L"../Bin/Resources/Mesh/DynamicMesh/scythe/",
		L"scythe.X"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STAGE,
		L"Mesh_Trap1",
		Engine::TYPE_DYNAMIC,
		L"../Bin/Resources/Mesh/DynamicMesh/Trap1/",
		L"Trap1.X"), E_FAIL);

	//FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
	//	RESOURCE_STAGE,
	//	L"Mesh_Navi",
	//	Engine::TYPE_NAVI, NULL, NULL),
	//	E_FAIL);

	lstrcpy(m_szLoading, L"Loading Complete...........");

	m_bFinish = true;

	return 0;
}

_uint CLoading::Thread_Main(void * pArg)
{
	CLoading* pLoading = (CLoading*)pArg;

	_uint iFlag = 0;

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;
	case LOADING_BOSS:
		break;
	default:
		break;
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	return iFlag;
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
	CLoading* pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eID)))
	{
		ERR_BOX("Loading Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Engine::Safe_Release(m_pGraphicDev);
}
