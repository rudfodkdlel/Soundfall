#pragma once


#include "Object_State.h"

NS_BEGIN(Client)
class CMonster_Pattern_Wall : public CObject_State
{
public:
	virtual ~CMonster_Pattern_Wall() = default;
public:
	// CObject_State을(를) 통해 상속됨
	void Enter(CGameObject* pObj, OBJTYPE eType) override;
	void Update(CGameObject* pObj, float fTimeDelta) override;
	void Exit(CGameObject* pObj) override;
	CObject_State* Check_Transition(CGameObject* pObj) override;

private:
	ATTACK m_eAttackState = { ATTACK::ATTACK_IN };
	_float		m_fDuration = { 1.5f };
	_float      m_fDelay = { 0.5f };
	_int		m_iCount = { 0 };
};

NS_END