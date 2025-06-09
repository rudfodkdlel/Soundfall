#include "Player_State_Combo.h"
#include "Player_State_Idle.h"
#include "Player.h"
#include "Model.h"
#include "Player_State_Move.h"
#include "Player_State_Dash.h"

void CPlayer_State_Combo::Enter(CGameObject* pObj, OBJTYPE eType)
{
    __super::Enter(pObj, eType);

	static_cast<CPlayer*>(pObj)->Get_Range_Weapon()->Set_Active(false);
	static_cast<CPlayer*>(pObj)->Get_Melee_Weapon()->Set_Active(true);

	m_pModel->Set_Animation(5, false);

	m_eAttackState = ATTACK::ATTACK_IN;
}

void CPlayer_State_Combo::Update(CGameObject* pObj, float fTimeDelta)
{
	// 콤보 타이밍만 판단
	if (m_pModel->Get_Current_Anim_Ratio() > 0.4f)
	{
		if (m_pGameInstance->Key_Up(DIK_SPACE))
			m_IsNextCombo = true;
	}

	
	if (m_IsNextCombo)
	{
		if (m_iComboCount == 0)
		{
			m_iComboCount = 1;
			m_pModel->Set_Animation(6, false);
			m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
			m_IsNextCombo = false;
				
		}
		else if (m_iComboCount == 1)
		{
			m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
			m_eAttackState = ATTACK::ATTACK_OUT;

			if (fabs(m_pGameInstance->Get_Timing() < 0.15f))
			{
				m_pModel->Set_Animation(8, false);
			}
			else
			{
				m_pModel->Set_Animation(7, false);
			}
			
			m_IsNextCombo = false;
				
		}
		
	}
	
	
	
}

void CPlayer_State_Combo::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
	static_cast<CPlayer*>(pObj)->Get_Range_Weapon()->Set_Active(true);
	static_cast<CPlayer*>(pObj)->Get_Melee_Weapon()->Set_Active(false);
}

CObject_State* CPlayer_State_Combo::Check_Transition(CGameObject* pObj)
{
	
	if (static_cast<CObserver_Animation*>(m_pGameInstance->
		Find_Observer(TEXT("Observer_Animation_Player")))
		->IsAnimationFinished())
	{
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
		return new CPlayer_State_Idle;
	}
	
    return nullptr;
}
