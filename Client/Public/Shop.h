#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Item_Icon.h"

NS_BEGIN(Client)

class CShop final : public CBase
{

private:
	CShop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CShop() = default;

public:
	CGameObject* Get_Select_Item();
	vector<CItem_Icon*>& Get_Items() { return m_Items; }

public:
	HRESULT Initialize();
	void Update();

	void Buy_Item();
	void Remove_Item();


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


	CGameObject* m_pTarget = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	vector<CItem_Icon*> m_Items = {};
	// 선택한 아이템의 인덱스
	_int m_iItemIndex = {-1};

public:
	static CShop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();


};

NS_END