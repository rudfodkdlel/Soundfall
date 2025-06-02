#pragma once


#include "Object_State.h"

NS_BEGIN(Client)
class CMonster_Pattern_Spawn : public CObject_State
{
public:
	virtual ~CMonster_Pattern_Spawn() = default;
public:
	// CObject_State을(를) 통해 상속됨
	void Enter(CGameObject* pObj, OBJTYPE eType) override;
	void Update(CGameObject* pObj, float fTimeDelta) override;
	void Exit(CGameObject* pObj) override;
	CObject_State* Check_Transition(CGameObject* pObj) override;

private:
	_int m_iSpawnCount = {5};
	_float m_fDelay = { 0.3f };
};

NS_END