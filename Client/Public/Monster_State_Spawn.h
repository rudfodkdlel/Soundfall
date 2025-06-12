#pragma once

#include "Monster_State_Base.h"

NS_BEGIN(Client)

class CMonster_State_Spawn : public CMonster_State_Base
{
public:
	virtual ~CMonster_State_Spawn() = default;

public:
	virtual void Enter(CGameObject* pObj) override;
	virtual void Update(CGameObject* pObj, float fTimeDelta);
	virtual void Exit(CGameObject* pObj);
	virtual CObject_State* Check_Transition(CGameObject* pObj);

private:
	_bool	m_IsAttack = { false };
	_float  m_fTime = { 2.f };

};

NS_END