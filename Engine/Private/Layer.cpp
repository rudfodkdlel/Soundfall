#include "Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CComponent* CLayer::Get_Component(const _wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	
	return (*iter)->Get_Component(strComponentTag);
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && false == pGameObject->Get_Dead())
			pGameObject->Priority_Update(fTimeDelta);

		if (nullptr != pGameObject && pGameObject->Get_Dead())
			Safe_Release(pGameObject);

	}
		
}

void CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && false == pGameObject->Get_Dead())
			pGameObject->Update(fTimeDelta);

		if (nullptr != pGameObject && pGameObject->Get_Dead())
			Safe_Release(pGameObject);
	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && false == pGameObject->Get_Dead())
			pGameObject->Late_Update(fTimeDelta);

		if (nullptr != pGameObject && pGameObject->Get_Dead())
			Safe_Release(pGameObject);
	}
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);
	m_GameObjects.clear();
}
