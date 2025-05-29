#pragma once

#include "Weapon_Base.h"

NS_BEGIN(Client)

class CRifle : public CWeapon_Base
{
private:
	CRifle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRifle(const CRifle& Prototype);
	virtual ~CRifle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Attack() override;

public:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CRifle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END