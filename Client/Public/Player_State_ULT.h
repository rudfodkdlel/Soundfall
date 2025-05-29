#pragma once

#include "Player_State_Attack.h"

class CPlayer_State_ULT : public CPlayer_State_Attack
{
public:
	virtual ~CPlayer_State_ULT() = default;

public:
	// CObject_State을(를) 통해 상속됨
	void Enter(CGameObject* pObj, OBJTYPE eType) override;
	void Update(CGameObject* pObj, float fTimeDelta) override;
	void Exit(CGameObject* pObj) override;
	CObject_State* Check_Transition(CGameObject* pObj) override;

private:
	_float		m_fDuration = { 1.5f };
	
};

