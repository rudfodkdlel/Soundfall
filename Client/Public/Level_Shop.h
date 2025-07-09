#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Shop.h"
#include "Inventory.h"
#include "Navi_Icon.h"
#include "Item_Icon.h"

NS_BEGIN(Client)

class CLevel_Shop final : public CLevel
{
private:
	CLevel_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Shop() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Buy_Item();
	void Change_Item();

private:
	HRESULT Ready_Layer_BackGround(const _wstring strLayerTag);
	HRESULT Ready_Layer_Shop_UI(const _wstring strLayerTag);

private:

	CNavi_Icon* m_pNavi_Shop = {nullptr};
	CNavi_Icon* m_pNavi_Inven = { nullptr };
	CNavi_Icon* m_pNavi_Buy = { nullptr };
	CNavi_Icon* m_pNavi_Change = { nullptr };

	CShop* m_pShop = {nullptr};
	CInventory* m_pInventory = { nullptr };

	vector<CItem_Icon*> m_EquipIcons = {};

public:
	static CLevel_Shop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END