// Management
// Get
CComponent*		Get_Component(const _tchar* pLayertag,
	const _tchar* pObjtag,
	const _tchar* ComponentKey,
	COMPONENTID eID)
{
	return CManagement::GetInstance()->Get_Component(pLayertag, pObjtag, ComponentKey, eID);
}

CLayer*			Get_Layer(const _tchar* pLayertag)
{
	return CManagement::GetInstance()->Get_Layer(pLayertag);
}
// Set
HRESULT		SetUp_CurrentScene(CScene* pCurrentScene)
{
	return CManagement::GetInstance()->SetUp_CurrentScene(pCurrentScene);
}
// General
HRESULT	Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement** ppManagement)
{
	CManagement*			pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	if (FAILED(pManagement->Ready_ShaderFile(pGraphicDev)))
		return E_FAIL;

	*ppManagement = pManagement;

	return S_OK;
}

HRESULT	Delete_GameObject(const _tchar * pLayertag, const _tchar * pObjtag)
{
	return CManagement::GetInstance()->Delete_GameObject(pLayertag, pObjtag);
}

HRESULT	Erase_GameObject(const _tchar* pLayertag, const _tchar* pObjtag, CGameObject* pGameObject)
{
	return CManagement::GetInstance()->Erase_GameObject(pLayertag, pObjtag, pGameObject);
}

HRESULT Add_Light(LPDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9& LightInfo, const _uint& iIndex)
{
	return CLightMgr::GetInstance()->Add_Light(pGraphic_Device, LightInfo, iIndex);
}

HRESULT Render_Light_Manager(LPD3DXEFFECT pEffect)
{
	return CLightMgr::GetInstance()->Render_Light_Manager(pEffect);
}

// ProtoMgr
// Get
// Set
// General
HRESULT		Ready_Prototype(const _tchar* pProtoTag, CComponent* pInstance)
{
	return CProtoMgr::GetInstance()->Ready_Prototype(pProtoTag, pInstance);
}
CComponent*	Clone(const _tchar* pProtoTag)
{
	return CProtoMgr::GetInstance()->Clone(pProtoTag);
}

HRESULT		Clear_Renderer()
{
	return CProtoMgr::GetInstance()->Clear_Renderer();
}


// KeyMgr
CKeyMgr* Get_KeyMgr(void)
{
	return CKeyMgr::GetInstance();
}

// TargetMgr
CTarget_Manager* Get_TargetMgr(void)
{
	return CTarget_Manager::GetInstance();
}

// Release
void		Release_Utility(void)
{
	CKeyMgr::GetInstance()->DestroyInstance();
	CProtoMgr::GetInstance()->DestroyInstance();
	CTarget_Manager::GetInstance()->DestroyInstance();
	CLightMgr::GetInstance()->DestroyInstance();
	CManagement::GetInstance()->DestroyInstance();
}