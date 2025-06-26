#include "Player_State_Hit.h"
#include "Player_State_Idle.h"

void CPlayer_State_Hit::Enter(CGameObject* pObj)
{
	__super::Enter(pObj);
	m_pPlayer->Toggle_Collider(false);


	m_pModel->Set_Animation(m_pPlayer->Get_Range_Weapon()->Get_HitReactMap()[m_pPlayer->Get_Hit_Dir()], false);

}

void CPlayer_State_Hit::Update(CGameObject* pObj, float fTimeDelta)
{
	m_IsFinish = m_pModel->Play_Animation(fTimeDelta);
}

void CPlayer_State_Hit::Exit(CGameObject* pObj)
{
	
	__super::Exit(pObj);
}

CObject_State* CPlayer_State_Hit::Check_Transition(CGameObject* pObj)
{
	if (m_IsFinish)
	{
		m_pPlayer->Toggle_Collider(true);
		m_pPlayer->Get_CombatCom()->Set_bInvinsible(false);
		return new CPlayer_State_Idle;
	}

	return nullptr;
}
