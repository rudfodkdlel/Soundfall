#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Forest final : public CLevel
{
private:
	CLevel_Forest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Forest() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CSpawner* Get_Spawner() { return m_pSpawner; }
	void  Set_IsChange() { m_IsChangeLevel = true; }

private:
	HRESULT Ready_Layer_BackGround(const _wstring strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring strLayerTag);
	HRESULT Ready_Layer_Player();
	HRESULT Ready_Lights();
	HRESULT Ready_Map_Data(const char* pFliePath);

private:
	class CSpawner* m_pSpawner = { nullptr };
	_bool m_IsChangeLevel = {false};

public:
	static CLevel_Forest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END