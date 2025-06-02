#pragma once


#include "Object_State.h"

NS_BEGIN(Client)
class CMonster_Pattern_Melee : public CObject_State
{
public:
	virtual ~CMonster_Pattern_Melee() = default;
public:
	// CObject_State을(를) 통해 상속됨
	void Enter(CGameObject* pObj, OBJTYPE eType) override;
	void Update(CGameObject* pObj, float fTimeDelta) override;
	void Exit(CGameObject* pObj) override;
	CObject_State* Check_Transition(CGameObject* pObj) override;

private:
	ATTACK m_eAttackState = { ATTACK::ATTACK_IN };
	_float		m_fDuration = { 1.5f };
	DIR_STATE m_eDir = { DIR_STATE::NONE };
	_int		m_iAnimnum = {};
};

NS_END