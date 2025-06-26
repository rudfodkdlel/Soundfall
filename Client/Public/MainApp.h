#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*				m_pGameInstance = { };
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

public:
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Font();
	// 콜라이더 그룹 설정해서 넣어주기? 등등 
	HRESULT Ready_Collision_Setting();
	HRESULT Ready_Static();


	HRESULT Start_Level(LEVEL eStartLevel);

public:
	static CMainApp* Create();
	virtual void Free() override;

};

NS_END