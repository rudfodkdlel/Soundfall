
#pragma once

#include "Monster_State_Base.h"

NS_BEGIN(Client)

class CMonster_Pattern_Missile : public CMonster_State_Base
{
public:
	virtual ~CMonster_Pattern_Missile() = default;

public:
	virtual void Enter(CGameObject* pObj) override;
	virtual void Update(CGameObject* pObj, float fTimeDelta);
	virtual void Exit(CGameObject* pObj);
	virtual CObject_State* Check_Transition(CGameObject* pObj);

private:
	_float		m_fDuration = { 1.5f };
	_float      m_fDelay = { 0.3f };
};

NS_END