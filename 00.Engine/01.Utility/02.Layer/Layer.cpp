#include "Layer.h"
#include "LightMgr.h"

USING(Engine)

Engine::CLayer::CLayer(void)
{

}

Engine::CLayer::~CLayer(void)
{

}

HRESULT Engine::CLayer::Ready_Layer(void)
{
	return S_OK;
}

_int Engine::CLayer::Update_Layer(const _float& fTimeDelta)
{
	_int		isEnd = 0;

	for (auto& iter : m_mapObject)
	{
		list<CGameObject*>::iterator iterList = iter.second.begin();
		for (; iterList != iter.second.end(); )
		{
			isEnd = (*iterList)->Update_Object(fTimeDelta); // 0 ,-1, 1

			if (isEnd == 1) // 삭제
			{
				Safe_Release((*iterList));
				iterList = iter.second.erase(iterList);
			}
			else if (isEnd & 0x80000000)
				return isEnd;
			else
				++iterList;
		}
	}

	return isEnd;
}

void Engine::CLayer::Render_Layer(void)
{
	for (auto& iter : m_mapObject)
	{
		for (auto& pObj : iter.second)
		{
			pObj->Render_Object();
		}
	}
}

HRESULT CLayer::Add_GameObject(const _tchar * pObjtag, CGameObject * pGameObject)
{
	if (nullptr == pGameObject ||
		nullptr == pObjtag)
		return E_FAIL;

	for (auto& Pair : m_mapObject)
	{
		if (!lstrcmp(Pair.first, pObjtag))
		{
			m_mapObject[Pair.first].emplace_back(pGameObject);
			return S_OK;
		}
	}

	m_mapObject[pObjtag].emplace_back(pGameObject);

	return S_OK;
}

CComponent * CLayer::Get_Component(const _tchar * pObjtag, const _tchar * ComponentKey, COMPONENTID eID)
{
	auto iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjtag));

	if (iter == m_mapObject.end())
		return nullptr;

	auto ObjIter = iter->second.begin();

	return (*ObjIter)->Get_Component(ComponentKey, eID); // 일단 첫번째 게임 오브젝트의 컴포넌트를 반환.
}

HRESULT CLayer::Delete_GameObject(const _tchar * pObjtag)
{
	auto iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjtag));

	if (iter == m_mapObject.end())
		return E_FAIL;

	Safe_Release(*(iter->second.begin())); // 처음꺼 삭제
	m_mapObject[pObjtag].erase(iter->second.begin());

	return S_OK;
}

HRESULT CLayer::Erase_GameObject(const _tchar * pObjtag, CGameObject* pGameObject)
{
	auto iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjtag));

	if (iter == m_mapObject.end())
		return E_FAIL;

	auto iter_begin = iter->second.begin();
	auto iter_end = iter->second.end();

	for (; iter_begin != iter_end; )
	{
		if (pGameObject == *iter_begin)
			iter_begin = iter->second.erase(iter_begin);
		else
			iter_begin++;
	}

//	m_mapObject[pObjtag].erase(iter->second.begin());
	return S_OK;
}

_bool CLayer::Empty_GameObject(const _tchar * pObjTag)
{
	auto iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjTag));

	if (iter == m_mapObject.end())
		return false;

	if (iter->second.empty())				// 만약 비어있다면 true
		return true;



	return false;
}

_bool CLayer::CollisionObject(const _tchar * pObjtag, const _matrix& matSphere, const _float& fRadius, const CGameObject::HIT_OPTION& eHitOption)
{
	auto iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjtag));

	if (iter == m_mapObject.end())
	{
		ERR_BOX("CollisionObject Func : pObjTag find Failed");
		return false;
	}

	for (auto& pObj : iter->second)
	{
		if(pObj->Get_Hit() || CGameObject::DIE == pObj->Get_State())	// 맞은 상태거나 죽은 상태라면
			continue;

		if (pObj->CollisionSphere(matSphere, fRadius)) // 구 충돌 유무
		{
			pObj->Set_State(CGameObject::DAMAGE);
			pObj->Set_Hit();	// 맞음으로 처리.
			pObj->Set_HitOption(eHitOption); // 맞았을때 나오는 애니메이션과 동작이 다를수 있다.
		}
	}

	return false;
}

HRESULT CLayer::Reset_HitObject(const _tchar * pObjtag)
{
	auto iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjtag));

	if (iter == m_mapObject.end())
	{
		ERR_BOX("Reset_HitObject Func : pObjTag find Failed");
		return false;
	}

	for (auto& pObj : iter->second)
	{
		pObj->Reset_Hit();
	}

	return NOERROR;
}

void CLayer::Open_Door(const _uint & iGroup)
{
	for (auto& Pair : m_mapObject)
	{
		for (auto& pGameObject : Pair.second)
		{
			if (iGroup == pGameObject->Get_Group())
				pGameObject->Set_Open();
		}
	}

	CLightMgr::GetInstance()->Set_LightIndex(iGroup);
	m_iGroupCount = iGroup;
}

_uint CLayer::Count_Group(const _uint & iGroup)
{
	_uint iCount = 0;
	for (auto& Pair : m_mapObject)
	{
		for (auto& pGameObject : Pair.second)
		{
			if (iGroup == pGameObject->Get_Group())
				++iCount;
		}
	}
	return iCount;		// 그룹이 1이 나와야 문만 남았다는의미이다.
}

Engine::CLayer* Engine::CLayer::Create(void)
{
	CLayer* pInstance = new CLayer;

	if (FAILED(pInstance->Ready_Layer()))
	{
		ERR_BOX("CLayer Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CLayer::Free(void)
{
	for (auto& ObjList : m_mapObject)
	{
		for_each(ObjList.second.begin(), ObjList.second.end(), CDeleteObj());
		ObjList.second.clear();
	}
	m_mapObject.clear();
}
