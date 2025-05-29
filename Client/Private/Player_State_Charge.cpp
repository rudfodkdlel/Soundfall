#include "Player_State_Charge.h"
#include "Player_State_Idle.h"
#include "Player.h"
#include "Model.h"
#include "Player_State_Move.h"
#include "Player_State_Dash.h"

void CPlayer_State_Charge::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);
	

	static_cast<CPlayer*>(pObj)->Get_Range_Weapon()->Set_Active(false);
	static_cast<CPlayer*>(pObj)->Get_Melee_Weapon()->Set_Active(true);

	m_pModel->Set_Animation(15, true);
	m_eAttackState = ATTACK::ATTACK_IN;
}

void CPlayer_State_Charge::Update(CGameObject* pObj, float fTimeDelta)
{
	if (m_eAttackState == ATTACK::ATTACK_IN)
	{
		if (m_pGameInstance->Key_Up(DIK_SPACE))
		{
			m_pModel->Set_Animation(17, true);
			m_eAttackState = ATTACK::ATTACK_LOOP;
		}
		else if (m_pGameInstance->Key_Pressing(DIK_SPACE))
		{
			m_fDuration += fTimeDelta;
			m_fDuration = max(m_fDuration, 0.5f);
		}
	}
	else if (ATTACK::ATTACK_LOOP == m_eAttackState)
	{
		m_fDuration -= fTimeDelta;

		if (m_fDuration < 0.f)
		{
			m_eAttackState = ATTACK::ATTACK_OUT;
			m_pModel->Set_Animation(20, false);
		}
	}
	else if(ATTACK::ATTACK_OUT == m_eAttackState)
	{

	}
}

void CPlayer_State_Charge::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
	static_cast<CPlayer*>(pObj)->Get_Range_Weapon()->Set_Active(true);
	static_cast<CPlayer*>(pObj)->Get_Melee_Weapon()->Set_Active(false);
}

CObject_State* CPlayer_State_Charge::Check_Transition(CGameObject* pObj)
{
	if (ATTACK::ATTACK_OUT == m_eAttackState &&
		static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player")))->IsAnimationFinished())
	{
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
		return new CPlayer_State_Idle;
	}


	return nullptr;
}
