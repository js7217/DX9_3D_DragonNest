#ifndef Export_System_h__
#define Export_System_h__

#include "Engine_Defines.h"
#include "GraphicDev.h"
#include "TimeMgr.h"
#include "FrameMgr.h"
#include "FontManager.h"
#include "InputDev.h"

BEGIN(Engine)

// GraphicDev
// Get
// Set
// Public
inline HRESULT		Ready_GraphicDev(HWND hWnd,	CGraphicDev::WINMODE eMode,	const _uint& iSizeX,	const _uint& iSizeY,	Engine::CGraphicDev** ppGraphicDev);

// TimeMgr
// Get
inline _float		Get_TimeDelta(const _tchar* pTimerTag);
// Set
inline void		SetUp_TimeDelta(const _tchar* pTimerTag);
// Public
inline HRESULT	Ready_Timer(const _tchar* pTimerTag);

// FrameMgr
// Get
// Set
// Public
inline	_bool	IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);
inline	HRESULT	Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit);

// FontMgr
// Get
// Set
// Public
inline HRESULT	Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
	const _tchar* pFontTag,
	const _tchar* pFontType,
	const _uint& iWidth,
	const _uint& iHeight,
	const _uint& iWeight);

inline void	Render_Font(const _tchar* pFontTag,
	const _tchar* pString,
	const _vec2* pPos,
	D3DXCOLOR Color);

// InputDev
// Get
inline _byte	Get_DIKeyState(_ubyte byKeyID);
inline _byte	Get_DIMouseState(CInputDev::MOUSEKEYSTATE eMouse);
inline _long	Get_DIMouseMove(CInputDev::MOUSEMOVESTATE eMouseState);

// Set
inline void	Set_InputDev(void);
// Public
inline HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);

// Release
inline void Release_System(void);


#include "Export_System.inl"

END
#endif // Export_System_h__

