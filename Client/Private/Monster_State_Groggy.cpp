#include "Monster_State_Groggy.h"
#include "Body_Discord.h"
#include "Monster_State_Idle.h"
#include "Monster_State_Death.h"

void CMonster_State_Groggy::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);

    auto colliders = static_cast<CBody_Discord*>(m_pDiscord->Get_Body())->Get_Colliders();

    colliders[0]->Set_Active(true);

	m_pDiscord->Get_CombatCom()->Set_Damage(0);

    m_pModel->Set_Animation(23, false);
}

void CMonster_State_Groggy::Update(CGameObject* pObj, float fTimeDelta)
{
	if (m_eAnimState == ANIMSTATE::ANIM_IN)
	{
		if (m_IsFinish)
		{
			m_eAnimState = ANIMSTATE::ANIM_LOOP;
			m_pModel->Set_Animation(24, false);
		}
	}
	else if (m_eAnimState == ANIMSTATE::ANIM_LOOP)
	{
		m_fDuration -= fTimeDelta;

		if (m_IsFinish)
		{
			eDirState eDir = m_pDiscord->Check_Head_HitDir();

			switch (eDir)
			{
			case F:
				m_pModel->Set_Animation(27, false);
				break;
			case B:
				m_pModel->Set_Animation(27, false);
				break;
			case R:
				m_pModel->Set_Animation(29, false);
				break;
			case L:
				m_pModel->Set_Animation(28, false);
				break;
			default:
				m_pModel->Set_Animation(24, false);
				break;
			}
		}

		
	}

	if (m_eAnimState == ANIMSTATE::ANIM_IN || m_eAnimState == ANIMSTATE::ANIM_LOOP)
	{
		if (m_fDuration < 0 || m_pDiscord->Get_CombatCom()->Get_Current_HP() <= 0)
		{
			m_eAnimState = ANIMSTATE::ANIM_OUT;
			m_pModel->Set_Animation(25, false);
		}
	}


    m_IsFinish = m_pModel->Play_Animation(fTimeDelta);
}

void CMonster_State_Groggy::Exit(CGameObject* pObj)
{
    __super::Exit(pObj);
}

CObject_State* CMonster_State_Groggy::Check_Transition(CGameObject* pObj)
{
	if (m_pDiscord->Get_Phase() == 2)
	{
		if (m_pDiscord->Get_CurrentHp() <= 0)
		{
			
			return new CMonster_State_Death;
		}
	}

	if (ANIMSTATE::ANIM_OUT == m_eAnimState && m_IsFinish)
	{
		if (m_pDiscord->Get_Phase() < 2)
		{
			auto colliders = static_cast<CBody_Discord*>(m_pDiscord->Get_Body())->Get_Colliders();

			colliders[0]->Set_Active(false);

			if (m_pDiscord->Get_CurrentHp() <= 0 && !m_isPhaseEnd)
			{
				m_isPhaseEnd = true;
				m_pDiscord->Get_CombatCom()->Heal(1.f);
				m_pDiscord->Add_Phase();
			}


			

			return new CMonster_State_Idle;
		}

	}




	return nullptr;
}
