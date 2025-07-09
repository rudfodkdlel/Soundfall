#include "Shop.h"



CShop::CShop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

CGameObject* CShop::Get_Select_Item()
{
	if(-1 == m_iItemIndex)
		return nullptr;

	return m_Items[m_iItemIndex];
}

HRESULT CShop::Initialize()
{
	// 아이템좀 미리 만들어 두는 식으로

	CItem_Icon::ITEM_UI_DESC eItemDesc = {};

	for (int j = 0; j < 5; ++j)
	{
		for (int i = 0; i < 4; ++i)
		{
			eItemDesc.fX = 875 + 100 * i;
			eItemDesc.fY = g_iWinSizeY * 0.6f - 200 + 100 * j;
			eItemDesc.fSizeX = 75.f;
			eItemDesc.fSizeY = 75.f;
			eItemDesc.iTextureType = _int(floorf(m_pGameInstance->Compute_Random(0.f, 6.9f)));

			if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Item_Icon"),
				static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Item_Icon"), &eItemDesc)))
				return E_FAIL;
		}
	}

	// 찾아서 다 넣어주기

	auto pObjList = m_pGameInstance->GetLayerList(static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Item_Icon"));

	for (auto& pObj : *pObjList)
	{
		m_Items.push_back(static_cast<CItem_Icon*>(pObj));
	}

	// 타겟 추가?

	return S_OK;
}

void CShop::Update()
{
	if (m_pGameInstance->Key_Down(DIM::LBUTTON))
	{
		for (int i = 0; i < m_Items.size(); ++i)
		{
			if (m_Items[i]->Check_Click())
			{
				if (m_iItemIndex != -1)
				{
					m_Items[m_iItemIndex]->Set_isClick(false);
				}

				m_iItemIndex = i;
				m_Items[i]->Set_isClick(true);
			}
		}
	}
	
}

void CShop::Buy_Item()
{

}

void CShop::Remove_Item()
{
	if (-1 == m_iItemIndex)
		return;

	(m_Items[m_iItemIndex])->Set_Dead();
}



CShop* CShop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShop* pInstance = new CShop(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CShop");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShop::Free()
{
	__super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);

}
