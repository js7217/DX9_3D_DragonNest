#ifndef AniCtrl_h__
#define AniCtrl_h__

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAniCtrl : public CBase
{
private:
	explicit CAniCtrl(LPD3DXANIMATIONCONTROLLER pAniCtrl);
	explicit CAniCtrl(const CAniCtrl& rhs);
	virtual ~CAniCtrl(void);

public:
	LPD3DXANIMATIONCONTROLLER	Get_AnimationSet(void) { return m_pAniCtrl; }
	_bool						Is_AnimationSetEnd(_double EndTime = 0.1);
	_bool						Is_AnimationAfterTrack(_double AfterTime);
	const _uint&				Get_AnimationNum();
	_double						Get_CurAnimationPosition();
public:
	HRESULT Ready_AniCtrl(void);
	void	Set_AnimationSet(const _uint& iIndex);
	void	Play_AnimationSet(const _float& fTimeDelta);

private:
	LPD3DXANIMATIONCONTROLLER		m_pAniCtrl; // �ִϸ��̼� ��Ʈ�ѷ�
	_uint							m_iCurrentTrack; // ���� �ִϸ��̼��� Ʈ��

	_uint							m_iOldAniIndex; // ���� �ִϸ��̼��� �ε�����. ������ ���� ���� �ִϸ��̼��� �ε������ ������ �ʿ���� ������ ����ó���� ���
	_uint							m_iNewTrack; // ������ ���� �ִϸ��̼� Ʈ��. ������ �̸� �ִϸ��̼��� �ε� �� ���¿��� ������ ������ �����ϴ� ����� ����Ѵ�. ���� Ʈ���� �ϳ� �� �ʿ��ϴ�.
	_float							m_fAccTime; // ������Ű�� ���� �ð���.

	_double							m_dPeriod; // �ִϸ��̼��� ������ ����(Position)

public:
	static CAniCtrl*	Create(LPD3DXANIMATIONCONTROLLER pAniCtrl);
	static CAniCtrl*	Create(const CAniCtrl& rhs);
	virtual void		Free(void);
};
END
#endif // AniCtrl_h__
