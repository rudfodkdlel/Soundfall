#pragma once
#include "Player_State_Base.h"

NS_BEGIN(Client)

class CPlayer_State_Attack_Charge : public CPlayer_State_Base
{
public:
	virtual ~CPlayer_State_Attack_Charge() = default;
public:
	// CObject_State을(를) 통해 상속됨
	virtual void Enter(CGameObject* pObj) override;
	virtual void Update(CGameObject* pObj, float fTimeDelta) override;
	virtual void Exit(CGameObject* pObj) override;
	virtual CObject_State* Check_Transition(CGameObject* pObj) override;

private:
	ATTACK	m_eAttackState = { ATTACK::ATTACK_IN };
	_float  m_fElapsedTime = {0.f};
	_vector m_vDir = {};
};

NS_END





