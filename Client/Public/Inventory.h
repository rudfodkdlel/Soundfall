#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Item_Icon.h"
#include "Player.h"
#include "Weapon_Base.h"

NS_BEGIN(Client)

class CInventory final : public CBase
{

private:
	CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CInventory() = default;

public:
	// 이거 기반으로 플레이어에서 무기를 만들자
	vector<CItem_Icon*>& Get_Equip_Icons() { return m_EquipIcons; }
	vector<CItem_Icon*>& Get_Inven_Icons() { return m_ICons; }
	_int Get_Equip_Index() { return m_iEquipItemIndex; }
	void Set_Equip_Index(_int iIndex) { m_iEquipItemIndex = iIndex; }
	_int Get_Inven_Index() { return m_iItemIndex; }

public:
	HRESULT Initialize(CGameObject* pObj);
	void Update();

	void Change_Item();

	void Add_Item(CItem_Icon* pIcon);
	
	HRESULT Save_Data();
	HRESULT Load_Data();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


	CPlayer* m_pTarget = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	vector<CWeapon_Base*> m_EquipWeapons = {};

	vector<CItem_Icon*> m_EquipIcons = {};

	vector<CItem_Icon*> m_ICons = {};
	// 선택한 아이템의 인덱스
	_int m_iItemIndex = { -1 };

	_int m_iEquipItemIndex = { -1 };

	_int m_iMoney = {};


	_bool m_isEmptyFile = { false };

public:
	static CInventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameObject* pObj);
	void Free();


};

NS_END