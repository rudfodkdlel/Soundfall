#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CWeapon_Defender : public CPartObject
{
public:
	typedef struct tagWeaponDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
	}WEAPON_DESC;
private:
	CWeapon_Defender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Defender(const CWeapon_Defender& Prototype);
	virtual ~CWeapon_Defender() = default;

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


public:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Defender* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END