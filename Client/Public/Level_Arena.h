#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Arena final : public CLevel
{
private:
	CLevel_Arena(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Arena() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const _wstring strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring strLayerTag);
	HRESULT Ready_Layer_Player();
	HRESULT Ready_Lights();
	
private:
	class CSpawner* m_pSpawner = { nullptr };
	_bool m_IsChangeLevel = { false };

	_float m_fLimitTime = { 30.f };
	_float m_fElapsedTime = { 0.f };

public:
	static CLevel_Arena* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END