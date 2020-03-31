#pragma once
#include "Engine_Defines.h"

const DWORD KEY_W = 0x00000001;	// 0000 0000 0000 0000 0000 0000 0000 0001
const DWORD KEY_S = 0x00000002;	// 0000 0000 0000 0000 0000 0000 0000 0010
const DWORD KEY_A = 0x00000004;	// 0000 0000 0000 0000 0000 0000 0000 0100
const DWORD KEY_D = 0x00000008;	// 0000 0000 0000 0000 0000 0000 0000 1000

const DWORD KEY_RETURN	= 0x00000010;	// 0000 0000 0000 0000 0000 0000 0001 0000
const DWORD KEY_SPACE	= 0x00000020;	// 0000 0000 0000 0000 0000 0000 0010 0000
const DWORD KEY_LBUTTON = 0x00000040;	// 0000 0000 0000 0000 0000 0000 0100 0000
const DWORD KEY_RBUTTON = 0x00000080;	// 0000 0000 0000 0000 0000 0000 1000 0000

const DWORD KEY_SHIFT	= 0x00000100;	// 0000 0000 0000 0000 0000 0000 1000 0000

BEGIN(Engine)

class ENGINE_DLL CKeyMgr
{
	DECLARE_SINGLETON(Engine::CKeyMgr)

private:
	CKeyMgr();
	~CKeyMgr();

public:
	void CheckKeyInput();
	bool KeyDown(DWORD dwKey);
	bool KeyUp(DWORD dwKey);
	bool KeyPressing(DWORD dwKey);
	bool KeyCombine(DWORD dwFirstKey, DWORD dwSecondKey);

private:
	DWORD m_dwCurKey;
	DWORD m_dwKeyDowned;
	DWORD m_dwKeyPressed;
};
END