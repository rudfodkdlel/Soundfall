#pragma once

#include "Object_State.h"
#include "Player.h"

NS_BEGIN(Client)

class CPlayer_State_Base : public CObject_State
{
public:
	virtual ~CPlayer_State_Base() = default;

public:
	virtual void Enter(CGameObject* pObj) override;
	virtual void Update(CGameObject* pObj, float fTimeDelta) {};
	virtual void Exit(CGameObject* pObj);
	virtual CObject_State* Check_Transition(CGameObject* pObj) { return nullptr; }

protected:
	CPlayer*  m_pPlayer = { nullptr };

};

NS_END