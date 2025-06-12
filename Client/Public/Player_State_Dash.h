#pragma once
#include "Player_State_Base.h"

NS_BEGIN(Client)

class CPlayer_State_Dash : public CPlayer_State_Base
{
public:
	virtual ~CPlayer_State_Dash() = default;
public:
	// CObject_State을(를) 통해 상속됨
	virtual void Enter(CGameObject* pObj) override;
	virtual void Update(CGameObject* pObj, float fTimeDelta) override;
	virtual void Exit(CGameObject* pObj) override;
	virtual CObject_State* Check_Transition(CGameObject* pObj) override;

private:
	_vector	m_vDir = {};
};

NS_END





