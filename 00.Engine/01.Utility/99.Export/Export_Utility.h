#ifndef Export_Utility_h__
#define Export_Utility_h__

#include "Management.h"
#include "Transform.h"
#include "Renderer.h"
#include "Calculator.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "BoneCollider.h"
#include "LightMgr.h"
#include "ProtoMgr.h"
#include "KeyMgr.h"
#include "Target_Manager.h"

BEGIN(Engine)

// Management
// Get
inline CComponent*		Get_Component(const _tchar* pLayertag,
	const _tchar* pObjtag,
	const _tchar* ComponentKey,
	COMPONENTID eID);

inline CLayer*			Get_Layer(const _tchar* pLayertag);
// Set
inline HRESULT		SetUp_CurrentScene(CScene* pCurrentScene);
// General
inline HRESULT	Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement** ppManagement);
inline HRESULT	Delete_GameObject(const _tchar * pLayertag, const _tchar * pObjtag);
inline HRESULT	Erase_GameObject(const _tchar* pLayertag, const _tchar* pObjtag, CGameObject* pGameObject);

// LightMgr
inline 	HRESULT Add_Light(LPDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9& LightInfo, const _uint& iIndex);
inline HRESULT Render_Light_Manager(LPD3DXEFFECT pEffect);
// ProtoMgr
// Get
// Set
// General
inline HRESULT		Ready_Prototype(const _tchar* pProtoTag, CComponent* pInstance);
inline CComponent*	Clone(const _tchar* pProtoTag);
inline HRESULT		Clear_Renderer();

// KeyMgr
inline CKeyMgr* Get_KeyMgr(void);

// TargetMgr
inline CTarget_Manager* Get_TargetMgr(void);
// Release
inline void		Release_Utility(void);

#include "Export_Utility.inl"

END
#endif // Export_Utility_h__
