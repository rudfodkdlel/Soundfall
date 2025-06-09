#include "Monster_State_Idle.h"
#include "Monster_Base.h"
#include "Discord.h"
#include "Observer_Animation.h"
#include "Observer_Phase.h"
#include "Monster_Pattern_EchoSlam.h"
#include "Monster_Pattern_Spawn.h"
#include "Monster_Pattern_Missile.h"
#include "Monster_Pattern_Melee.h"
#include "Monster_Pattern_Wall.h"
#include "Monster_State_Groggy.h"
#include "Body_Discord.h"

void CMonster_State_Idle::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);
	

	m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();


	m_pModel = static_cast<CMonster_Base*>(pObj)->Get_BodyModel();
	Safe_AddRef(m_pModel);

	int iPhase = static_cast<CDiscord*>(pObj)->Get_Phase();


	if (1 == iPhase)
	{
		m_pModel->Set_Animation(30, false);
			
	}
	else  if (2 == iPhase)
	{
		m_pModel->Set_Animation(31, false);
	}
	else if (3 == iPhase)
	{
		m_pModel->Set_Animation(32, false);
	}
	
}

void CMonster_State_Idle::Update(CGameObject* pObj, float fTimeDelta)
{
}

void CMonster_State_Idle::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CMonster_State_Idle::Check_Transition(CGameObject* pObj)
{
	if (static_cast<CDiscord*>(pObj)->Get_CombatCom()->Get_Current_HP() <= 0)
	{
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
		static_cast<CDiscord*>(pObj)->Set_bUseSpawn(false);
		return new CMonster_State_Groggy;
	}

	if (static_cast<CDiscord*>(pObj)->Check_Groggy())
	{
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
		static_cast<CDiscord*>(pObj)->Set_bUseSpawn(false);
		return new CMonster_State_Groggy;
	}


	if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
	{
		//m_eNextPattern = static_cast<CDiscord*>(pObj)->Get_Next_Skill();
		// BOSS_PATTERN(int(m_pGameInstance->Compute_Random(0.f, 5.f)))
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
		switch (static_cast<CDiscord*>(pObj)->Get_Next_Skill())
		{
		case Client::BP_MELEE:
			return new CMonster_Pattern_Melee;
			break;
		case Client::BP_ECHOSLAM:
			return new CMonster_Pattern_EchoSlam;
			break;
		case Client::BP_SPAWN:
			static_cast<CDiscord*>(pObj)->Set_bUseSpawn(true);
			return new CMonster_Pattern_Spawn;
			break;
		case Client::BP_WALL:
			return new CMonster_Pattern_Wall;
			break;
		case Client::BP_MISSILE:
			return new CMonster_Pattern_Missile;
			break;
		default:
			break;
		}
	}



	return nullptr;
}
