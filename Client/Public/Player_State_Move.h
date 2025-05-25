#pragma once


#include "Object_State.h"

NS_BEGIN(Client)
class Player_State_Move : public CObject_State
{
public:
	virtual ~Player_State_Move() = default;
public:
	// CObject_State을(를) 통해 상속됨
	void Enter(CGameObject* pObj) override;
	void Update(CGameObject* pObj, float fTimeDelta) override;
	void Exit(CGameObject* pObj) override;

};

NS_END