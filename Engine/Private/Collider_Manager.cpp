#include "Collider_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"

CCollider_Manager::CCollider_Manager()
{
}

HRESULT CCollider_Manager::Add_Collider(_uint iIndex, CCollider* Collider, CGameObject* pOwner)
{ 
	if (nullptr == Collider)
		return S_OK;

	if (m_pColliders.size() <= iIndex)
		m_pColliders.resize(iIndex + 1);

	m_pColliders[iIndex].push_back({ Collider, pOwner });

	//Safe_AddRef(Collider);
	//Safe_AddRef(pOwner);

	Collider->Set_Group(iIndex);

	return S_OK;
}

HRESULT CCollider_Manager::Add_Collider_Group(pair<_uint, _uint> typePair)
{
	m_CollisionPairs.push_back(typePair);

	_uint maxIndex = 0;

	for (const auto& pair : m_CollisionPairs) {
		_uint temp = max(pair.first, pair.second);

		maxIndex = max(maxIndex, temp);
	}

	m_pColliders.resize(maxIndex + 1);

    return S_OK;
}

void CCollider_Manager::Check_Nullptr()
{
	for (auto& list : m_pColliders)
	{
		list.remove_if([](std::pair<CCollider*, CGameObject*>& p)
			{
				if (p.first == nullptr || p.second == nullptr || p.second->Get_Dead())
				{
					//Safe_Release(p.first);
					//Safe_Release(p.second);
					return true;
				}
				return false;
			});
	}
	
}

void CCollider_Manager::Clear()
{
	for (auto& list : m_pColliders)
	{
		for (auto& ptr : list)
		{
			if(nullptr != ptr.first)
				Safe_Release(ptr.first);
			if(nullptr != ptr.second)
				Safe_Release(ptr.second);
		}
		list.clear();
	}

}

void CCollider_Manager::Update()
{

	if (m_pColliders.empty())
		return;

	for (auto& pair : m_CollisionPairs)
	{
		if (m_pColliders[pair.first].empty() || m_pColliders[pair.second].empty())
			continue;

		
		
		Instercects_Group(m_pColliders[pair.first], m_pColliders[pair.second]);
	}

	//Check_Nullptr();

}

void CCollider_Manager::Instercects_Group(list<pair<CCollider*, CGameObject*>> src, list<pair<CCollider*, CGameObject*>> dst)
{
	vector<tuple<CGameObject*, CGameObject*, CCollider*, CCollider*>> collisions;

	for (auto& srcCollider : src)
	{
		if (nullptr == srcCollider.first || nullptr == srcCollider.second)
			continue;
		if (!srcCollider.first->Get_Active() || srcCollider.second->Get_Dead())
			continue;

		for (auto& dstCollider : dst)
		{
			if (nullptr == dstCollider.first || nullptr == dstCollider.second)
				continue;
			if (!dstCollider.first->Get_Active() || dstCollider.second->Get_Dead())
				continue;
			if (srcCollider.second == dstCollider.second)
				continue;

			if (srcCollider.first->Intersect(dstCollider.first))
			{
				// 충돌 목록에 저장만 해둠
				collisions.emplace_back(
					srcCollider.second, dstCollider.second,
					srcCollider.first, dstCollider.first);
			}
		}
	}

	// 이후 일괄 처리
	for (auto& collision : collisions)
	{
		CGameObject* srcObj = get<0>(collision);
		CGameObject* dstObj = get<1>(collision);
		CCollider* srcCol = get<2>(collision);
		CCollider* dstCol = get<3>(collision);

		if (srcObj->Get_Dead() || dstObj->Get_Dead())
			continue;

		srcObj->On_Collision(dstObj, dstCol);
		dstObj->On_Collision(srcObj, srcCol);
	}
}

CCollider_Manager* CCollider_Manager::Create()
{
	return new CCollider_Manager();
}

void CCollider_Manager::Free()
{
	__super::Free();

	//Clear();

	m_pColliders.clear();
	m_CollisionPairs.clear();
}
