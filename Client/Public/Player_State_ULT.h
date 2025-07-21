#pragma once

#include "Player_State_Base.h"

class CPlayer_State_Ult : public CPlayer_State_Base
{
public:
	virtual ~CPlayer_State_Ult() = default;

public:
	// CObject_State을(를) 통해 상속됨
	void Enter(CGameObject* pObj) override;
	void Update(CGameObject* pObj, float fTimeDelta) override;
	void Exit(CGameObject* pObj) override;
	CObject_State* Check_Transition(CGameObject* pObj) override;

private:
	_float		m_fDuration = { 1.5f };
	WEAPON		m_eWeaponType = {};
	ATTACK		m_eAttackState = {};
	_float4		m_vMousePos = {};
	_float4     m_vDir = {};
	_bool		m_bMakeEffect = {false};

};


