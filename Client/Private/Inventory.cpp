#include "Inventory.h"

CInventory::CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}


HRESULT CInventory::Initialize(CGameObject* pObj)
{
	ofstream fout("../Bin/Data/Inventory.bin", std::ios::in || std::ios::binary);
	if (!fout)
	{
		if (nullptr != pObj)
		{
			m_pTarget = static_cast<CPlayer*>(pObj); 

			m_isEmptyFile = true;
			// 파일이 없으면 임의 값 박을거야

			return S_OK;
		}
	}

	// 상점에서는 데이터 가져와야 되니까
	if (nullptr == pObj)
	{
		m_pTarget = nullptr;
		Load_Data();
	}
		



    return S_OK;
}

void CInventory::Update()
{

	
	// equip 말고 업데이트 하기
	if (m_pGameInstance->Key_Down(DIM::LBUTTON))
	{
		for (int i = 0; i < m_ICons.size(); ++i)
		{
			if (nullptr != m_ICons[i])
			{
				if (m_ICons[i]->Check_Click())
				{
					if (m_iItemIndex != -1)
					{
						m_ICons[m_iItemIndex]->Set_isClick(false);
					}

					m_iItemIndex = i;
					m_ICons[i]->Set_isClick(true);
				}
			}
		}
	}

		
	
}

void CInventory::Change_Item()
{
	if (-1 == m_iEquipItemIndex || -1 == m_iItemIndex)
		return;

	// 위치를 서로 바꾸고, 포인터 위치도 바꿔준다

	auto EquipPos = m_EquipIcons[m_iEquipItemIndex]->Get_Pos();
	auto InvenPos = m_ICons[m_iItemIndex]->Get_Pos();

	m_EquipIcons[m_iEquipItemIndex]->Change_Position(InvenPos.first, InvenPos.second);
	m_ICons[m_iItemIndex]->Change_Position(EquipPos.first, EquipPos.second);

	CItem_Icon* pTemp = {};
	pTemp = m_EquipIcons[m_iEquipItemIndex];
	m_EquipIcons[m_iEquipItemIndex] = m_ICons[m_iItemIndex];
	m_ICons[m_iItemIndex] = pTemp;
	
}

void CInventory::Add_Item(CItem_Icon* pIcon)
{
	WEAPON_ICON_INFO_DESC eDesc = pIcon->Get_Weapon_Info_Desc();

	// index 따라 다시 만든다...

	_int iIndex = m_ICons.size();


	CItem_Icon::ITEM_UI_DESC eItemDesc = {};

	eItemDesc.fSizeX = 75.f;
	eItemDesc.fSizeY = 75.f;
	eItemDesc.fX = 875 + 100 * ((iIndex) % 4);
	eItemDesc.fY = g_iWinSizeY * 0.6f - 200 + 100 * int(iIndex / 4);
	eItemDesc.iTextureType = eDesc.iTextureType;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Item_Icon"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Inventory_Icon"), &eItemDesc)))
		return;

	auto pObj = m_pGameInstance->GetLastObjectFromLayer(static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Inventory_Icon"));

	m_ICons.push_back(static_cast<CItem_Icon*>(pObj));
	Safe_AddRef(m_ICons.back());
}



HRESULT CInventory::Save_Data()
{

	if (nullptr == m_pTarget)
	{
		// 상점에서는 정보가 바뀌니까 다시 다 고쳐쓴다
		ofstream fout("../Bin/Data/Inventory.bin", std::ios::binary);
		if (!fout)
		{
			return E_FAIL;
		}

		fout.write(reinterpret_cast<const char*>(&m_iMoney), sizeof(_int));

		_int iSize = m_EquipIcons.size();

		fout.write(reinterpret_cast<const char*>(&iSize), sizeof(_int));

		for (int i = 0; i < m_EquipIcons.size(); ++i)
		{
			if (nullptr != m_EquipIcons[i])
			{
				auto eWeaponDesc = m_EquipIcons[i]->Get_Weapon_Info_Desc();

				fout.write(reinterpret_cast<const char*>(&eWeaponDesc.iTextureType), sizeof(_int));
				eWeaponDesc.iIndex = i;
				fout.write(reinterpret_cast<const char*>(&eWeaponDesc.iIndex), sizeof(_int));

				Safe_Release(m_EquipIcons[i]);
			}
		}


		iSize = m_ICons.size();

		fout.write(reinterpret_cast<const char*>(&iSize), sizeof(_int));

		for (int i = 0; i < m_ICons.size(); ++i)
		{
			if (nullptr != m_ICons[i])
			{
				auto eWeaponDesc = m_ICons[i]->Get_Weapon_Info_Desc();

				fout.write(reinterpret_cast<const char*>(&eWeaponDesc.iTextureType), sizeof(_int));
				eWeaponDesc.iIndex = i;
				fout.write(reinterpret_cast<const char*>(&eWeaponDesc.iIndex), sizeof(_int));

				Safe_Release(m_ICons[i]);
			}

		}

		fout.close();
	}
	else
	{
		// 상점이 아닐 경우,  파일이 없을때만 다시 만든다
		ofstream fout("../Bin/Data/Inventory.bin", std::ios::in || std::ios::binary);
		if (!fout)
		{
			ofstream fout("../Bin/Data/Inventory.bin", std::ios::binary);
			if (!fout)
			{
				return E_FAIL;
			}

			m_iMoney = m_pTarget->Get_Money();

			fout.write(reinterpret_cast<const char*>(&m_iMoney), sizeof(_int));

			_int iSize = 4;
			fout.write(reinterpret_cast<const char*>(&iSize), sizeof(_int));

			WEAPON_ICON_INFO_DESC eDesc = {};

			eDesc.iIndex = 0;
			eDesc.iTextureType = 0;

			fout.write(reinterpret_cast<const char*>(&eDesc.iTextureType), sizeof(_int));
			fout.write(reinterpret_cast<const char*>(&eDesc.iIndex), sizeof(_int));

			eDesc.iIndex = 1;
			eDesc.iTextureType = 4;

			fout.write(reinterpret_cast<const char*>(&eDesc.iTextureType), sizeof(_int));
			fout.write(reinterpret_cast<const char*>(&eDesc.iIndex), sizeof(_int));

			eDesc.iIndex = 2;
			eDesc.iTextureType = 7;

			fout.write(reinterpret_cast<const char*>(&eDesc.iTextureType), sizeof(_int));
			fout.write(reinterpret_cast<const char*>(&eDesc.iIndex), sizeof(_int));

			eDesc.iIndex = 3;
			eDesc.iTextureType = 8;

			fout.write(reinterpret_cast<const char*>(&eDesc.iTextureType), sizeof(_int));
			fout.write(reinterpret_cast<const char*>(&eDesc.iIndex), sizeof(_int));

			iSize = m_ICons.size();

			fout.write(reinterpret_cast<const char*>(&iSize), sizeof(_int));

			fout.close();
		}

	}



    return S_OK;
}

HRESULT CInventory::Load_Data()
{
	std::ifstream fin("../Bin/Data/Inventory.bin", std::ios::binary);
	if (!fin)
	{
		
		return S_OK;
	}

	fin.read(reinterpret_cast<char*>(&m_iMoney), sizeof(_int));

	_int iSize = {};

 	fin.read(reinterpret_cast<char*>(&iSize), sizeof(_int));
	
	// equip부터

	for (int i = 0; i < iSize; ++i)
	{
		
		WEAPON_ICON_INFO_DESC eDesc = {};

		fin.read(reinterpret_cast<char*>(&eDesc.iTextureType), sizeof(_int));

		fin.read(reinterpret_cast<char*>(&eDesc.iIndex), sizeof(_int));

		CItem_Icon::ITEM_UI_DESC eItemDesc = {};

		eItemDesc.fX = 100;
		eItemDesc.fSizeX = 75.f;
		eItemDesc.fSizeY = 75.f;

		eItemDesc.fY = g_iWinSizeY * (0.4f + 0.15f * eDesc.iIndex);

		eItemDesc.iTextureType = eDesc.iTextureType;

		if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Item_Icon"),
			static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Equip_Icon"), &eItemDesc)))
			return E_FAIL;
		
	}

	if (iSize > 0)
	{
		auto pObjList = m_pGameInstance->GetLayerList(static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Equip_Icon"));

		for (auto& pObj : *pObjList)
		{
			static_cast<CItem_Icon*>(pObj)->Set_isRender(true);
			Safe_AddRef(pObj);

			m_EquipIcons.push_back(static_cast<CItem_Icon*>(pObj));
		}
			
	}
	

	// inven에 있는 거

	fin.read(reinterpret_cast<char*>(&iSize), sizeof(_int));

	for (int i = 0; i < iSize; ++i)
	{
		WEAPON_ICON_INFO_DESC eDesc = {};

		fin.read(reinterpret_cast<char*>(&eDesc.iTextureType), sizeof(_int));

		fin.read(reinterpret_cast<char*>(&eDesc.iIndex), sizeof(_int));

		CItem_Icon::ITEM_UI_DESC eItemDesc = {};

		eItemDesc.fSizeX = 75.f;
		eItemDesc.fSizeY = 75.f;

		eItemDesc.fX = 875 + 100 * (eDesc.iIndex % 4);
		eItemDesc.fY = g_iWinSizeY * 0.6f - 200 + 100 * int(eDesc.iIndex / 4);

		eItemDesc.iTextureType = eDesc.iTextureType;

		if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Item_Icon"),
			static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Inventory_Icon"), &eItemDesc)))
			return E_FAIL;

		auto pObj = m_pGameInstance->GetLastObjectFromLayer(static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Inventory_Icon"));

		m_ICons.push_back(static_cast<CItem_Icon*>(pObj));
	}
	

	for (auto& pObj : m_ICons)
	{
		Safe_AddRef(pObj);

	}
		
	

	fin.close();
	
    return S_OK;
}

CInventory* CInventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameObject* pObj)
{
	CInventory* pInstance = new CInventory(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed to Created : CInventory");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInventory::Free()
{

	__super::Free();


	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);

	if (FAILED(Save_Data()))
	{
		MSG_BOX("Failed to Save Inventory Data");
	}
}