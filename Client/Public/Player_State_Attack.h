#pragma once
#include "Object_State.h"

class CPlayer_State_Attack : public CObject_State
{
public:
	virtual ~CPlayer_State_Attack() = default;
public:
	// CObject_State을(를) 통해 상속됨
	void Enter(CGameObject* pObj, OBJTYPE eType) override;
	void Update(CGameObject* pObj, float fTimeDelta) override;
	void Exit(CGameObject* pObj) override;
	CObject_State* Check_Transition(CGameObject* pObj) override;

protected:
	_vector	  m_vDir = {};
	ATTACK    m_eAttackState = { ATTACK::ATTACK_IN };
	WEAPON	  m_eWeaponType = { WEAPON::END };
};

