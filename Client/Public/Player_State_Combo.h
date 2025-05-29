#pragma once

#include "Player_State_Attack.h"

class CPlayer_State_Combo : public CPlayer_State_Attack
{
public:
	virtual ~CPlayer_State_Combo() = default;

public:
	// CObject_State을(를) 통해 상속됨
	void Enter(CGameObject* pObj, OBJTYPE eType) override;
	void Update(CGameObject* pObj, float fTimeDelta) override;
	void Exit(CGameObject* pObj) override;
	CObject_State* Check_Transition(CGameObject* pObj) override;

private:
	_int		m_iComboCount = { 0 };
	_bool		m_IsNextCombo = { false };
};
