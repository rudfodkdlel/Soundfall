#pragma once

#include "Monster_State_Base.h"

NS_BEGIN(Client)

class CMonster_State_Idle : public CMonster_State_Base
{
public:
	virtual ~CMonster_State_Idle() = default;

public:
	virtual void Enter(CGameObject* pObj) override;
	virtual void Update(CGameObject* pObj, float fTimeDelta);
	virtual void Exit(CGameObject* pObj);
	virtual CObject_State* Check_Transition(CGameObject* pObj);

private:
	_int m_iPhase;
	_float m_fSpeedRatio = {};
};

NS_END