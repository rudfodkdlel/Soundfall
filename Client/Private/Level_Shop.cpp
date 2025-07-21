#include "Level_Shop.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Level_Loading.h"
#include "Shop.h"
#include "Static_UI.h"

CLevel_Shop::CLevel_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel{pDevice, pContext}
{
}

HRESULT CLevel_Shop::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Shop_UI(TEXT("Layer_Shop_UI"))))
		return E_FAIL;

		m_pShop = CShop::Create(m_pDevice, m_pContext);

		m_pInventory = CInventory::Create(m_pDevice, m_pContext, nullptr);

		m_EquipIcons = m_pInventory->Get_Equip_Icons();
		
		

		for (auto& pObj : m_EquipIcons)
			static_cast<CItem_Icon*>(pObj)->Set_isRender(true);

		m_pGameInstance->StopSound(SOUND_BGM);
		m_pGameInstance->PlaySound(TEXT("TutorialMusic.ogg"), SOUND_BGM, 0.3f);
	

	return S_OK;
}

void CLevel_Shop::Update(_float fTimeDelta)
{


	if (m_pGameInstance->Key_Down(DIM::LBUTTON))
	{
		_bool bClickInven = m_pNavi_Inven->Check_Click();
		_bool bClickShop = m_pNavi_Shop->Check_Click();

		if (bClickInven && !bClickShop)
		{
			m_pNavi_Inven->Set_isClick(true);
			m_pNavi_Shop->Set_isClick(false);

			for (auto& pObj : m_pShop->Get_Items())
				pObj->Set_isRender(false);

			for (auto& pObj : m_pInventory->Get_Inven_Icons())
				pObj->Set_isRender(true);

		}
		else if (!bClickInven && bClickShop)
		{
			m_pNavi_Inven->Set_isClick(false);
			m_pNavi_Shop->Set_isClick(true);

			for (auto& pObj : m_pShop->Get_Items())
				pObj->Set_isRender(true);

			for (auto& pObj : m_pInventory->Get_Inven_Icons())
				pObj->Set_isRender(false);

			
		}

		// 다른 버튼 눌렸는지 체크
		_bool bClickBuy = m_pNavi_Buy->Check_Click();
		_bool bClickChange = m_pNavi_Change->Check_Click();

		//

		if (bClickBuy)
		{
			if(nullptr != m_pShop->Get_Select_Item())
			m_pInventory->Add_Item(static_cast<CItem_Icon*>(m_pShop->Get_Select_Item()));
			m_pShop->Remove_Item();
		}

		if (bClickChange)
		{
			Change_Item();
		}

		
		for (int i = 0; i < m_EquipIcons.size(); ++i)
		{
			if (nullptr != m_EquipIcons[i])
			{
				if (m_EquipIcons[i]->Check_Click())
				{
					if (m_pInventory->Get_Equip_Index() != -1)
					{
						m_EquipIcons[m_pInventory->Get_Equip_Index()]->Set_isClick(false);
					}

					m_pInventory->Set_Equip_Index(i);
					m_EquipIcons[i]->Set_isClick(true);
				}
			}
		}
		

	}

	if (m_pNavi_Shop->Get_isClick())
	{
		m_pShop->Update();
	}
	else if (m_pNavi_Inven->Get_isClick())
	{
		m_pInventory->Update();
	}

}

HRESULT CLevel_Shop::Render()
{
	SetWindowText(g_hWnd, TEXT("Shop 레벨입니다."));

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(static_cast<_uint>(LEVEL::LOADING),
			CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
			return E_FAIL;
	}
	else if (GetKeyState('A') & 0x8000)
	{
		// go forest

		if (FAILED(m_pGameInstance->Change_Level(static_cast<_uint>(LEVEL::LOADING),
			CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::ARENA))))
			return E_FAIL;

	}

	else if (GetKeyState('F') & 0x8000)
	{
		// go forest

		if (FAILED(m_pGameInstance->Change_Level(static_cast<_uint>(LEVEL::LOADING),
			CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::FOREST))))
			return E_FAIL;

	}


	return S_OK;
}

void CLevel_Shop::Buy_Item()
{
	 
}

void CLevel_Shop::Change_Item()
{
	m_pInventory->Change_Item();
	m_EquipIcons = m_pInventory->Get_Equip_Icons();
}

HRESULT CLevel_Shop::Ready_Layer_BackGround(const _wstring strLayerTag)
{
	CBackGround::BACKGROUND_DESC				BackGroundDesc{};

	BackGroundDesc.fX = g_iWinSizeX * 0.5f;
	BackGroundDesc.fY = g_iWinSizeY * 0.5f;

	BackGroundDesc.fSizeX = g_iWinSizeX;
	BackGroundDesc.fSizeY = g_iWinSizeY;
	BackGroundDesc.iType = 1;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_BackGround"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_BackGround"), &BackGroundDesc)))
		return E_FAIL;

	CStatic_UI::STATIC_UI_DESC staticDesc{};

	staticDesc.fX = 100;
	staticDesc.fY = g_iWinSizeY * 0.2f;
	staticDesc.fSizeX = 150.f;
	staticDesc.fSizeY = 150.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Player_Ky");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_BackGround"), &staticDesc)))
		return E_FAIL;


	staticDesc.fX = 100;
	staticDesc.fY = g_iWinSizeY * 0.4f;
	staticDesc.fSizeX = 100.f;
	staticDesc.fSizeY = 100.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Item_BackGround");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_BackGround"), &staticDesc)))
		return E_FAIL;

	staticDesc.fX = 100;
	staticDesc.fY = g_iWinSizeY * 0.55f;
	staticDesc.fSizeX = 100.f;
	staticDesc.fSizeY = 100.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Item_BackGround");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_BackGround"), &staticDesc)))
		return E_FAIL;

	staticDesc.fX = 100;
	staticDesc.fY = g_iWinSizeY * 0.7f;
	staticDesc.fSizeX = 100.f;
	staticDesc.fSizeY = 100.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Item_BackGround");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_BackGround"), &staticDesc)))
		return E_FAIL;

	staticDesc.fX = 100;
	staticDesc.fY = g_iWinSizeY * 0.85f;
	staticDesc.fSizeX = 100.f;
	staticDesc.fSizeY = 100.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Item_BackGround");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_BackGround"), &staticDesc)))
		return E_FAIL;


	staticDesc.fX = g_iWinSizeX * 0.5f;
	staticDesc.fY = g_iWinSizeY * 0.5f;
	staticDesc.fSizeX = g_iWinSizeX * 0.8f;
	staticDesc.fSizeY = g_iWinSizeY;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Ky_FullBody");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_BackGround"), &staticDesc)))
		return E_FAIL;



	staticDesc.fX = 1025;
	staticDesc.fY = g_iWinSizeY * 0.6f;
	staticDesc.fSizeX = 400.f;
	staticDesc.fSizeY = 500.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Navi_Button");
	staticDesc.iPassIndex = 6;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_BackGround"), &staticDesc)))
		return E_FAIL;

	// 
	for (int j = 0; j < 5; ++j)
	{
		for (int i = 0; i < 4; ++i)
		{
			staticDesc.fX = 875 + 100 * i;
			staticDesc.fY = g_iWinSizeY * 0.6f - 200 + 100 * j;
			staticDesc.fSizeX = 80.f;
			staticDesc.fSizeY = 80.f;
			staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Item_BackGround");
			staticDesc.iPassIndex = 0;

			if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
				static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_BackGround"), &staticDesc)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_Shop_UI(const _wstring strLayerTag)
{
	// 버튼 추가

	CNavi_Icon::NAVI_UI_DESC eNaviDesc = {};

	eNaviDesc.fX = 900;
	eNaviDesc.fY = g_iWinSizeY * 0.2f;
	eNaviDesc.fSizeX = 135.f;
	eNaviDesc.fSizeY = 40.f;
	eNaviDesc.strCaption = TEXT("Inventory");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Navi_Icon"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Button"), &eNaviDesc)))
		return E_FAIL;

	auto pObj = m_pGameInstance->GetLastObjectFromLayer(static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Button"));

	m_pNavi_Inven = static_cast<CNavi_Icon*>(pObj);

	eNaviDesc.fX = 1050;
	eNaviDesc.fY = g_iWinSizeY * 0.2f;
	eNaviDesc.fSizeX = 75.f;
	eNaviDesc.fSizeY = 40.f;
	eNaviDesc.strCaption = TEXT("Store");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Navi_Icon"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Button"), &eNaviDesc)))
		return E_FAIL;

	pObj = m_pGameInstance->GetLastObjectFromLayer(static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Button"));

	m_pNavi_Shop = static_cast<CNavi_Icon*>(pObj);

	eNaviDesc.fX = 750;
	eNaviDesc.fY = g_iWinSizeY * 0.9f;
	eNaviDesc.fSizeX = 60.f;
	eNaviDesc.fSizeY = 40.f;
	eNaviDesc.strCaption = TEXT("Buy");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Navi_Icon"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Button"), &eNaviDesc)))
		return E_FAIL;

	pObj = m_pGameInstance->GetLastObjectFromLayer(static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Button"));

	m_pNavi_Buy = static_cast<CNavi_Icon*>(pObj);

	eNaviDesc.fX = 600;
	eNaviDesc.fY = g_iWinSizeY * 0.9f;
	eNaviDesc.fSizeX = 100.f;
	eNaviDesc.fSizeY = 40.f;
	eNaviDesc.strCaption = TEXT("Change");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Navi_Icon"),
		static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Button"), &eNaviDesc)))
		return E_FAIL;

	pObj = m_pGameInstance->GetLastObjectFromLayer(static_cast<_uint>(LEVEL::SHOP), TEXT("Layer_Button"));

	m_pNavi_Change = static_cast<CNavi_Icon*>(pObj);




	return S_OK;
}

CLevel_Shop* CLevel_Shop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Shop* pInstance = new CLevel_Shop(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Shop");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Shop::Free()
{
	__super::Free();

	Safe_Release(m_pShop);
	Safe_Release(m_pInventory);

	

}