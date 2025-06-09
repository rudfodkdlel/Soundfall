#pragma once
#include "Object_State.h"

NS_BEGIN(Client)

class CMonster_State_Groggy : public CObject_State
{
public:
	virtual ~CMonster_State_Groggy() = default;
public:
	// CObject_State을(를) 통해 상속됨
	void Enter(CGameObject* pObj, OBJTYPE eType) override;
	void Update(CGameObject* pObj, float fTimeDelta) override;
	void Exit(CGameObject* pObj) override;
	CObject_State* Check_Transition(CGameObject* pObj) override;

private:
	_float		m_fDuration = { 5.f };
	ANIMSTATE	m_eAnimState = { ANIMSTATE::ANIM_IN };
};

NS_END

