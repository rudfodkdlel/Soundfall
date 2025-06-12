#pragma once

#include "Weapon_Base.h"

NS_BEGIN(Client)

class CKeyboard : public CWeapon_Base
{
private:
	CKeyboard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKeyboard(const CKeyboard& Prototype);
	virtual ~CKeyboard() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Attack(_vector vDir) override;

	void Reset();

public:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_bool m_IsFire = { true };
	_int m_iCurrentAmmo = { 1 };
	_int m_iMaxAmmo = { 1 };
	// 4번 연속 틀리면 못쏘게 바꾸기
	_int m_iOverloadCount = { 4 };
	_float m_fOverloadTime = { 0.f };

public:
	static CKeyboard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END