#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CWeapon_Base abstract : public CPartObject
{
public:
	typedef struct tagWeaponDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
	}WEAPON_DESC;

protected:
	CWeapon_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Base(const CWeapon_Base& Prototype);
	virtual ~CWeapon_Base() = default;

public:
	WEAPON	Get_WeaponType() { return m_eWeaponType; }
	map < DIR_STATE, _int>& Get_DirMap() { return m_DirMap; }
	void	Set_Active(_bool isActive) { m_IsActive = isActive; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	// 근접은 콜라이더 껏다 키기, 원거리는 투사체 생성 이런 느낌으로?
	// 궁극기는 안에서 입력을 뭘로 받았는지 판단해서 ...? 여기서 부르는 느낌으로
	virtual void Attack() = 0;

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

protected:
	const _float4x4* m_pSocketMatrix = { nullptr };
	WEAPON			 m_eWeaponType = { WEAPON::END };
	map < DIR_STATE, _int> m_DirMap = {};

	// 일단 어떻게 사용할지 좀 더 생각해보기
	_bool			 m_IsActive = { true };
	_bool			 m_IsMelee = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;

};

NS_END