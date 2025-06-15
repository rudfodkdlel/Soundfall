#pragma once

#include "Object_State.h"
#include "Discord.h"

NS_BEGIN(Client)

class CMonster_State_Base : public CObject_State
{
public:
	virtual ~CMonster_State_Base() = default;

public:
	virtual void Enter(CGameObject* pObj) override;
	virtual void Update(CGameObject* pObj, float fTimeDelta) {};
	virtual void Exit(CGameObject* pObj);
	virtual CObject_State* Check_Transition(CGameObject* pObj) { return nullptr; }

protected:
	ATTACK m_eAttackState = { ATTACK::ATTACK_IN };
	CDiscord* m_pDiscord = { nullptr };

};

NS_END