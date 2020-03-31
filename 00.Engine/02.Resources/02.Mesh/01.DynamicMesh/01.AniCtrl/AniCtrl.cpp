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

	// ���� ����Ǵ� Ʈ�� ������ �������� �Լ�.
	m_pAniCtrl->GetTrackDesc(m_iCurrentTrack, &tTrackInfo);

	// ������ Ʈ�� ������ Position���� ���� Ʈ���� �� ������ ���� ���Ͽ� ��ȯ���� ������.
	// 0.1�� ���͹�, �� ������ �ð��� �ִ� �뵵.
	if (tTrackInfo.Position >= m_dPeriod - EndTime)
		return true;

	return false;
}

_bool CAniCtrl::Is_AnimationAfterTrack(_double AfterTime)
{
	D3DXTRACK_DESC tTrackInfo;
	ZeroMemory(&tTrackInfo, sizeof(D3DXTRACK_DESC));

	// ���� ����Ǵ� Ʈ�� ������ �������� �Լ�.
	m_pAniCtrl->GetTrackDesc(m_iCurrentTrack, &tTrackInfo);

	// ������ Ʈ�� ������ Position���� ���� Ʈ���� �� ������ ���� ���Ͽ� ��ȯ���� ������.
	// 0.1�� ���͹�, �� ������ �ð��� �ִ� �뵵.
	if (tTrackInfo.Position >= AfterTime)
		return true;

	return false;
}

const _uint & CAniCtrl::Get_AnimationNum()
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	return m_pAniCtrl->GetNumAnimationSets(); // �ִϸ��̼��� �� ������ ��ȯ.
}

_double CAniCtrl::Get_CurAnimationPosition()
{
	D3DXTRACK_DESC tTrackInfo;
	ZeroMemory(&tTrackInfo, sizeof(D3DXTRACK_DESC));

	// ���� ����Ǵ� Ʈ�� ������ �������� �Լ�.
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
		return; // �ִϸ��̼��� �����Ͱ� ������� ������ ���� ���� �ٽ� ���� �ʿ�� ���� ����.

	m_iNewTrack = m_iCurrentTrack == 0 ? 1 : 0; // �� Ʈ���� ��ȣ�� ���� �޶���Ѵ�. 0, 1. ��ġ�� �ȵ�.

	LPD3DXANIMATIONSET pAs = NULL;

	// �ִϸ��̼� ���� �ε����� �������� ���
	// m_pAniCtrl->GetAnimationSetByName(); �ִϸ��̼� ���� ���ڿ��� ã�� �������� �Լ�
	m_pAniCtrl->GetAnimationSet(iIndex, &pAs);

	// �ִϸ��̼��� ������ ������ üũ�ϱ� ���� �Լ�.
	// �̋� ��ȯ�ϴ� ���� ���� �ִϸ��̼� Ʈ���� �����ǰ� ��ġ�ϴ� ��
	m_dPeriod = pAs->GetPeriod();

	// 0�� Ʈ���� ���� �ִϸ��̼� ���� �÷����´�(�����Ѵ�)
	m_pAniCtrl->SetTrackAnimationSet(m_iNewTrack, pAs);

	// �޽� �ȿ� ���Ե� ���� ����Ʈ�� ���� ȿ������ ������� �ʰڴٰ� ����� ��������.
	// �̺�Ʈ ���� ������ ��ĩ ���� ������ �ȵǴ� ��쵵 �߻��ϱ� ����.
	m_pAniCtrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniCtrl->UnkeyAllTrackEvents(m_iNewTrack);

	// Ű ������ : �ִϸ��̼� �� ��� �� ��� ������ �ǹ��Ѵ�.(Ư�� �ð����� �����ִ� ���� ����)
	// �ʹ� ���� �����ӿ� ���� Ű �������� ��� ���۵��� ������ �ְ� ���� ������ ���� ������ �����ϰ� �ȴ�.

	// ���� ����ǰ� �ִ� ������ ���� �Ѵ� �Լ�.
	// 3���� : ���ڰ��� �ݵ�� double Ÿ���̾����, ����ǰ� �ִ� �ִϸ��̼� Ű �����ӿ� ���͹� �� ����.
	m_pAniCtrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fAccTime + 0.25);

	//0.25 ��� ���͹� ������ ����Ǵ� Ű�������� �ӵ��� �����ϴ� �Լ�.
	m_pAniCtrl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	// �ִϸ��̼� �� �ռ� �ÿ� ����ġ�� �������ִ� �Լ�.
	m_pAniCtrl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	m_pAniCtrl->SetTrackEnable(m_iNewTrack, TRUE);
	m_pAniCtrl->KeyTrackSpeed(m_iNewTrack, 1.f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);
	m_pAniCtrl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	m_pAniCtrl->ResetTime(); // AdvanceTime�Լ��� ȣ��Ǹ� �ִϸ��̼��� ����Ǵ� ���� ���������� �ð��� �����ϰ��ֱ� ������ �����Ѵ�.
	m_fAccTime = 0.f;

	// ���� ��� ���̴� Ʈ������ �� Ʈ������ ��ü�߱� ������ �ִϸ��̼� ��� ��ġ�� 0.0���� �ٲ۴�.
	m_pAniCtrl->SetTrackPosition(m_iNewTrack, 0.0);

	m_iOldAniIndex = iIndex;
	m_iCurrentTrack = m_iNewTrack;
}

void CAniCtrl::Play_AnimationSet(const _float & fTimeDelta)
{
	m_pAniCtrl->AdvanceTime(fTimeDelta, NULL);
	// �ִϸ��̼��� ����ϴ� �Լ�
	// 2��° ���� : �ִϸ��̼� ���ۿ� ���� ����Ʈ �Ǵ� ���� ��ü�� �ּ�
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
