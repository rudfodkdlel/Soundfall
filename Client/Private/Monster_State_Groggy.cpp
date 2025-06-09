#include "Monster_State_Groggy.h"
#include "Discord.h"
#include "Observer_Animation.h"
#include "Monster_State_Idle.h"
#include "Monster_State_Death.h"
#include "Body_Discord.h"

void CMonster_State_Groggy::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);


	m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();


	m_pModel = static_cast<CMonster_Base*>(pObj)->Get_BodyModel();
	Safe_AddRef(m_pModel);

	auto colliders = static_cast<CBody_Discord*>(static_cast<CDiscord*>(pObj)->Get_Body())->Get_Colliders();

	colliders[0]->Set_Active(true);

	static_cast<CDiscord*>(pObj)->Get_CombatCom()->Set_Damage(0);
	
	m_pModel->Set_Animation(23, false);
	
	
}

void CMonster_State_Groggy::Update(CGameObject* pObj, float fTimeDelta)
{
	auto pObserver = static_cast<CObserver_Animation*>(
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")));

	if (m_eAnimState == ANIMSTATE::ANIM_IN )
	{
		if (pObserver->IsAnimationFinished())
		{
			m_eAnimState = ANIMSTATE::ANIM_LOOP;
			pObserver->Reset();
			m_pModel->Set_Animation(24, true);
		}
	}
	else if (m_eAnimState == ANIMSTATE::ANIM_LOOP)
	{
		m_fDuration -= fTimeDelta;
	}

	if (m_eAnimState == ANIMSTATE::ANIM_IN || m_eAnimState == ANIMSTATE::ANIM_LOOP)
	{
		if (m_fDuration < 0 || static_cast<CDiscord*>(pObj)->Get_CombatCom()->Get_Current_HP() <= 0)
		{
			m_eAnimState = ANIMSTATE::ANIM_OUT;
			m_pModel->Set_Animation(25, false);
		}
	}
	

	
	
}

void CMonster_State_Groggy::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CMonster_State_Groggy::Check_Transition(CGameObject* pObj)
{
	auto pObserver = static_cast<CObserver_Animation*>(
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")));

	if (static_cast<CDiscord*>(pObj)->Get_Phase() == 3)
	{
		if (static_cast<CDiscord*>(pObj)->Get_CurrentHp() <= 0)
		{
			pObserver->Reset();
			return new CMonster_State_Death;
		}
	}

	if (ANIMSTATE::ANIM_OUT == m_eAnimState  && pObserver->IsAnimationFinished())
	{
		if (static_cast<CDiscord*>(pObj)->Get_Phase() < 3)
		{
			if (static_cast<CDiscord*>(pObj)->Get_CurrentHp() <= 0)
			{
				static_cast<CDiscord*>(pObj)->Get_CombatCom()->Heal(1.f);
				static_cast<CDiscord*>(pObj)->Add_Phase();
			}

			pObserver->Reset();

			auto colliders = static_cast<CBody_Discord*>(static_cast<CDiscord*>(pObj)->Get_Body())->Get_Colliders();

			colliders[0]->Set_Active(false);

			return new CMonster_State_Idle;
		}
	
	}


	

    return nullptr;
}
