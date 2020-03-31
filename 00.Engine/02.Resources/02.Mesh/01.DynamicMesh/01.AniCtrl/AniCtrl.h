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
	LPD3DXANIMATIONCONTROLLER		m_pAniCtrl; // 애니메이션 컨트롤러
	_uint							m_iCurrentTrack; // 현재 애니메이션의 트랙

	_uint							m_iOldAniIndex; // 예전 애니메이션의 인덱스값. 보간시 만약 같은 애니메이션의 인덱스라면 보간이 필요없기 때문에 예외처리에 사용
	_uint							m_iNewTrack; // 보간할 다음 애니메이션 트랙. 보간은 미리 애니메이션이 로드 된 상태에서 서로의 정점을 보간하는 방식을 사용한다. 따라서 트랙이 하나 더 필요하다.
	_float							m_fAccTime; // 누적시키기 위한 시간값.

	_double							m_dPeriod; // 애니메이션의 끝나는 시점(Position)

public:
	static CAniCtrl*	Create(LPD3DXANIMATIONCONTROLLER pAniCtrl);
	static CAniCtrl*	Create(const CAniCtrl& rhs);
	virtual void		Free(void);
};
END
#endif // AniCtrl_h__
