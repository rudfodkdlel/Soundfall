#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "CombatStat.h"

NS_BEGIN(Engine)
class CShader;
class CModel;

NS_END

NS_BEGIN(Client)

class CWeapon_Sniper : public CPartObject
{
public:
	typedef struct tagWeaponDesc : public CPartObject::PARTOBJECT_DESC
	{
		STATE_MAIN* eMainState;
		const _float4x4* pSocketMatrix = { nullptr };
	}WEAPON_DESC;
private:
	CWeapon_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Sniper(const CWeapon_Sniper& Prototype);
	virtual ~CWeapon_Sniper() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

protected:
	const _float4x4* m_pSocketMatrix = { nullptr };


private:
	_bool	m_IsColl = { false };
	STATE_MAIN* m_eMainState;
public:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Sniper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END