#include "KeyMgr.h"


USING(Engine)

IMPLEMENT_SINGLETON(CKeyMgr)

Engine::CKeyMgr::CKeyMgr()
	: m_dwCurKey(0), m_dwKeyDowned(0), m_dwKeyPressed(0)
{

}

Engine::CKeyMgr::~CKeyMgr()
{

}


void Engine::CKeyMgr::CheckKeyInput()
{
	m_dwCurKey = 0;

	if (GetAsyncKeyState('W') & 0x8000)
		m_dwCurKey |= KEY_W;
	if (GetAsyncKeyState('S') & 0x8000)
		m_dwCurKey |= KEY_S;
	if (GetAsyncKeyState('A') & 0x8000)
		m_dwCurKey |= KEY_A;
	if (GetAsyncKeyState('D') & 0x8000)
		m_dwCurKey |= KEY_D;
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		m_dwCurKey |= KEY_RETURN;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		m_dwCurKey |= KEY_SPACE;
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		m_dwCurKey |= KEY_LBUTTON;
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		m_dwCurKey |= KEY_RBUTTON;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		m_dwCurKey |= KEY_SHIFT;
}

bool Engine::CKeyMgr::KeyDown(DWORD dwKey)
{
	// 전에 누른적 없고 현재 눌렸을 때 true
	if (!(m_dwKeyDowned & dwKey) && (m_dwCurKey & dwKey))
	{
		m_dwKeyDowned |= dwKey;
		return true;
	}
	// 전에 누른적 있고 현재 누르지 않았을 때 false
	else if ((m_dwKeyDowned & dwKey) && !(m_dwCurKey & dwKey))
	{
		m_dwKeyDowned ^= dwKey;
		return false;
	}

	return false;
}

bool Engine::CKeyMgr::KeyUp(DWORD dwKey)
{
	// 전에 누른적 있고 현재 안 눌렸을 때 true
	if ((m_dwKeyPressed & dwKey) && !(m_dwCurKey & dwKey))
	{
		m_dwKeyPressed ^= dwKey;
		return true;
	}
	// 전에 누른적 없고 현재 눌렸을 때 false
	else if (!(m_dwKeyPressed & dwKey) && (m_dwCurKey & dwKey))
	{
		m_dwKeyPressed |= dwKey;
		return false;
	}

	return false;
}

bool Engine::CKeyMgr::KeyPressing(DWORD dwKey)
{
	if (m_dwCurKey & dwKey)
		return true;

	return false;
}

// Ctrl + C 또는 Ctrl + V와 같은 기능.
bool Engine::CKeyMgr::KeyCombine(DWORD dwFirstKey, DWORD dwSecondKey)
{
	if (KeyDown(dwSecondKey) && KeyPressing(dwFirstKey))
		return true;

	return false;
}