#pragma once
#include "Player_State_Base.h"
#include "Attack_Area_Player.h"
#include "Trail_Effect_Axe.h"

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
	_float  m_fMakeParticleTime = { 0.f };
	_vector m_vDir = {};

	CTrail_Effect_Axe* m_pTrail = { nullptr };
	_bool	m_isMakeEffect = { false };
};

NS_END





