#pragma once


#include "Object_State.h"

NS_BEGIN(Client)
class CPlayer_State_Move : public CObject_State
{
public:
	virtual ~CPlayer_State_Move() = default;
public:
	// CObject_State을(를) 통해 상속됨
	void Enter(CGameObject* pObj, OBJTYPE eType) override;
	void Update(CGameObject* pObj, float fTimeDelta) override;
	void Exit(CGameObject* pObj) override;
	CObject_State* Check_Transition(CGameObject* pObj) override;

private:
	_vector	m_vDir = {};
};

NS_END