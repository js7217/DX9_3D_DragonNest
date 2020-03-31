#include "AniCtrl.h"

USING(Engine)

CAniCtrl::CAniCtrl(LPD3DXANIMATIONCONTROLLER pAniCtrl)
	: m_pAniCtrl(pAniCtrl)
	, m_iCurrentTrack(0)
	, m_iNewTrack(1)
	, m_iOldAniIndex(99)
	, m_fAccTime(0.f)
{
	m_pAniCtrl->AddRef();
}

CAniCtrl::CAniCtrl(const CAniCtrl & rhs)
	: m_iCurrentTrack(rhs.m_iCurrentTrack)
	, m_fAccTime(rhs.m_fAccTime)
	, m_iNewTrack(rhs.m_iNewTrack)
	, m_iOldAniIndex(rhs.m_iOldAniIndex)
{
	rhs.m_pAniCtrl->CloneAnimationController(rhs.m_pAniCtrl->GetMaxNumAnimationOutputs(),
		rhs.m_pAniCtrl->GetMaxNumAnimationSets(),
		rhs.m_pAniCtrl->GetMaxNumTracks(),
		rhs.m_pAniCtrl->GetMaxNumEvents(),
		&m_pAniCtrl);
}

CAniCtrl::~CAniCtrl(void)
{
}

_bool CAniCtrl::Is_AnimationSetEnd(_double EndTime)
{
	D3DXTRACK_DESC tTrackInfo;
	ZeroMemory(&tTrackInfo, sizeof(D3DXTRACK_DESC));

	// 현재 재생되는 트랙 정보를 가져오는 함수.
	m_pAniCtrl->GetTrackDesc(m_iCurrentTrack, &tTrackInfo);

	// 가져온 트랙 정보의 Position값과 현재 트랙의 끝 지점을 서로 비교하여 반환값을 나눈다.
	// 0.1은 인터벌, 즉 보간할 시간을 주는 용도.
	if (tTrackInfo.Position >= m_dPeriod - EndTime)
		return true;

	return false;
}

_bool CAniCtrl::Is_AnimationAfterTrack(_double AfterTime)
{
	D3DXTRACK_DESC tTrackInfo;
	ZeroMemory(&tTrackInfo, sizeof(D3DXTRACK_DESC));

	// 현재 재생되는 트랙 정보를 가져오는 함수.
	m_pAniCtrl->GetTrackDesc(m_iCurrentTrack, &tTrackInfo);

	// 가져온 트랙 정보의 Position값과 현재 트랙의 끝 지점을 서로 비교하여 반환값을 나눈다.
	// 0.1은 인터벌, 즉 보간할 시간을 주는 용도.
	if (tTrackInfo.Position >= AfterTime)
		return true;

	return false;
}

const _uint & CAniCtrl::Get_AnimationNum()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_pAniCtrl->GetNumAnimationSets(); // 애니메이션의 총 개수를 반환.
}

_double CAniCtrl::Get_CurAnimationPosition()
{
	D3DXTRACK_DESC tTrackInfo;
	ZeroMemory(&tTrackInfo, sizeof(D3DXTRACK_DESC));

	// 현재 재생되는 트랙 정보를 가져오는 함수.
	m_pAniCtrl->GetTrackDesc(m_iCurrentTrack, &tTrackInfo);

	return tTrackInfo.Position;
}

HRESULT CAniCtrl::Ready_AniCtrl(void)
{
	return S_OK;
}

void CAniCtrl::Set_AnimationSet(const _uint & iIndex)
{
	if (m_iOldAniIndex == iIndex)
		return; // 애니메이션이 다음것과 현재것이 같으면 굳이 셋을 다시 해줄 필요는 없기 때문.

	m_iNewTrack = m_iCurrentTrack == 0 ? 1 : 0; // 각 트랙의 번호가 서로 달라야한다. 0, 1. 겹치면 안됨.

	LPD3DXANIMATIONSET pAs = NULL;

	// 애니메이션 셋을 인덱스로 가져오는 방법
	// m_pAniCtrl->GetAnimationSetByName(); 애니메이션 셋을 문자열로 찾아 가져오는 함수
	m_pAniCtrl->GetAnimationSet(iIndex, &pAs);

	// 애니메이션이 끝나는 지점을 체크하기 위한 함수.
	// 이떄 반환하는 값은 현재 애니메이션 트랙의 포지션과 일치하는 값
	m_dPeriod = pAs->GetPeriod();

	// 0번 트랙에 얻어온 애니메이션 셋을 올려놓는다(세팅한다)
	m_pAniCtrl->SetTrackAnimationSet(m_iNewTrack, pAs);

	// 메쉬 안에 삽입된 각종 이펙트와 같은 효과들을 사용하지 않겠다고 명령을 내려야함.
	// 이벤트 정보 때문에 자칫 선형 보간이 안되는 경우도 발생하기 때문.
	m_pAniCtrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniCtrl->UnkeyAllTrackEvents(m_iNewTrack);

	// 키 프레임 : 애니메이션 셋 재생 중 재생 지점을 의미한다.(특정 시간에서 멈춰있는 동작 상태)
	// 너무 많은 프레임에 따라 키 프레임의 몇몇 동작들을 가지고 있고 선형 보간을 통해 동작을 제어하게 된다.

	// 현재 진행되고 있는 동작을 끄고 켜는 함수.
	// 3인자 : 인자값은 반드시 double 타입이어야함, 재생되고 있던 애니메이션 키 프레임에 인터벌 값 제공.
	m_pAniCtrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fAccTime + 0.25);

	//0.25 라는 인터벌 값동안 재생되던 키프레임의 속도를 제어하는 함수.
	m_pAniCtrl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	// 애니메이션 셋 합성 시에 가중치를 지정해주는 함수.
	m_pAniCtrl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	m_pAniCtrl->SetTrackEnable(m_iNewTrack, TRUE);
	m_pAniCtrl->KeyTrackSpeed(m_iNewTrack, 1.f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);
	m_pAniCtrl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	m_pAniCtrl->ResetTime(); // AdvanceTime함수가 호출되면 애니메이션이 재생되는 동안 내부적으로 시간을 누적하고있기 때문에 리셋한다.
	m_fAccTime = 0.f;

	// 기존 재생 중이던 트랙에서 새 트랙으로 교체했기 때문에 애니메이션 재생 위치를 0.0으로 바꾼다.
	m_pAniCtrl->SetTrackPosition(m_iNewTrack, 0.0);

	m_iOldAniIndex = iIndex;
	m_iCurrentTrack = m_iNewTrack;
}

void CAniCtrl::Play_AnimationSet(const _float & fTimeDelta)
{
	m_pAniCtrl->AdvanceTime(fTimeDelta, NULL);
	// 애니메이션을 재생하는 함수
	// 2번째 인자 : 애니메이션 동작에 맞춰 이펙트 또는 사운드 객체의 주소
	m_fAccTime += fTimeDelta;
}

CAniCtrl * CAniCtrl::Create(LPD3DXANIMATIONCONTROLLER pAniCtrl)
{
	CAniCtrl* pInstance = new CAniCtrl(pAniCtrl);

	if (FAILED(pInstance->Ready_AniCtrl()))
		Safe_Release(pInstance);

	return pInstance;
}

CAniCtrl * CAniCtrl::Create(const CAniCtrl & rhs)
{
	CAniCtrl* pInstance = new CAniCtrl(rhs);

	if (FAILED(pInstance->Ready_AniCtrl()))
		Safe_Release(pInstance);

	return pInstance;
}

void CAniCtrl::Free(void)
{
	Safe_Release(m_pAniCtrl);
}
