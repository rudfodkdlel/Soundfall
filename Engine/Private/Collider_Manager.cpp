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

	m_pColliders[iIndex].push_back( Collider );

	Collider->Set_Group(iIndex);
	Collider->Set_Owner(pOwner);

	Collider->Set_InvalidateCallBack(
		[this](CCollider* p) {this->CallbackCollider(p); }
	);

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

void CCollider_Manager::CallbackCollider(CCollider* p)
{
	if (m_pColliders.empty())
		return;

	if (m_pColliders[p->Get_Group()].empty())
		return;

	auto& pList = m_pColliders[p->Get_Group()];

	for (auto& pCollider : pList)
	{
		if (pCollider == p)
		{
			pCollider = nullptr;
			break;
		}
	}
}

void CCollider_Manager::Check_Nullptr()
{
	for (auto& list : m_pColliders)
	{
		list.remove_if([](CCollider* p)
			{
				if (p == nullptr)
					return true;

				if (nullptr == p->Get_Owner())
				{
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
			if(nullptr != ptr)
				Safe_Release(ptr);
		}
		list.clear();
	}

}

void CCollider_Manager::Update()
{
	Check_Nullptr();


	if (m_pColliders.empty())
		return;

	for (auto& pair : m_CollisionPairs)
	{
		if (m_pColliders[pair.first].empty() || m_pColliders[pair.second].empty())
			continue;

		
		
		Instercects_Group(m_pColliders[pair.first], m_pColliders[pair.second]);
	}

	

}

void CCollider_Manager::Instercects_Group(list<CCollider*> src, list<CCollider*> dst)
{
	for (auto& pSrcCollider : src)
	{ 
		if (pSrcCollider->Get_Group() < 0)
			continue;
		if (nullptr == pSrcCollider || nullptr == pSrcCollider->Get_Owner())
			continue;

		if ( !pSrcCollider->Get_Active() || pSrcCollider->Get_Owner()->Get_Dead())
			continue;

		for (auto& pDstCollider : dst)
		{

			if (pDstCollider->Get_Group() < 0)
				continue;

			if (pSrcCollider == pDstCollider)
				continue;

			if (nullptr == pDstCollider || nullptr == pDstCollider->Get_Owner())
				continue;

			if (!pDstCollider->Get_Active() || pDstCollider->Get_Owner()->Get_Dead())
				continue;

			if (pSrcCollider->Intersect(pDstCollider))
			{
				if (nullptr != pSrcCollider->Get_Owner())
					pSrcCollider->Get_Owner()->On_Collision(pDstCollider);
				if (nullptr != pDstCollider->Get_Owner())
					pDstCollider->Get_Owner()->On_Collision(pSrcCollider);
			}

			
		}
	}
}

CCollider_Manager* CCollider_Manager::Create()
{
	return new CCollider_Manager();
}

void CCollider_Manager::Free()
{
	__super::Free();

	Clear();

	m_pColliders.clear();
	m_CollisionPairs.clear();
}
