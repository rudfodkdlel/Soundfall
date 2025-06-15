#pragma once

#include "Monster_State_Base.h"

NS_BEGIN(Client)

class CMonster_State_Death : public CMonster_State_Base
{
public:
	virtual ~CMonster_State_Death() = default;

public:
	virtual void Enter(CGameObject* pObj) override;
	virtual void Update(CGameObject* pObj, float fTimeDelta);
	virtual void Exit(CGameObject* pObj);
	virtual CObject_State* Check_Transition(CGameObject* pObj);

private:
	_float m_fDuration = { 3.f };
	ANIMSTATE	m_eAnimState = { ANIMSTATE::ANIM_IN };
};

NS_END