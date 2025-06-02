#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"
#include "Weapon_Base.h"
#include "Object_State.h"


NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CPlayer final : public CContainerObject
{
	enum PART_PLAYER { PART_BODY,  PART_EFFECT, PART_WEAPON_0, PART_WEAPON_1, PART_WEAPON_MELEE_0, PART_WEAPON_MELEE_1, PART_END };
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	_vector Get_Dir() { return m_vDir; }
	WEAPON	Get_WeaponType() { return m_pRangedWeapon->Get_WeaponType(); }
	class CModel* Get_BodyModel() { return static_cast<class CModel*>( m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Model"))); }
	CWeapon_Base* Get_Range_Weapon() { return m_pRangedWeapon; }
	CWeapon_Base* Get_Melee_Weapon() { return m_pMeleeWeapon; }
	void		  Set_Weapon_Active(_bool IsActive) { m_pRangedWeapon->Set_Active(IsActive); }
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	void Input_Key(_float fTimeDelta);
	void Look_Mouse();
	void Select_State();
	
	

	// 상태 제어를 위한 함수들
	_bool Check_Move();
	DIR_STATE Calc_Dir(_vector vDir);
	void Move_Pos(_vector& vDir, _float fTimeDelta, _float fSpeedRatio = 1.f);
	void Select_Weapon();
	

private:
	CObject_State* m_pState = { nullptr };
	DIR_STATE	   m_eDirState = { NONE };

	CWeapon_Base*	m_pRangedWeapon = { nullptr };
	CWeapon_Base*	m_pMeleeWeapon = { nullptr };

	// inventory 만들어서 담아두자
	//class CInventory* m_pInventory = { nullptr };

	_vector	m_vDir = {};
	_float  m_fSpeed = {};



private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END