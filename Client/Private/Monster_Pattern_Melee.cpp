#include "Monster_Pattern_Melee.h"
#include "Monster_Base.h"
#include "Monster_State_Idle.h"
#include "Discord.h"

void CMonster_Pattern_Melee::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);

	m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();


	m_pModel = static_cast<CMonster_Base*>(pObj)->Get_BodyModel();
	Safe_AddRef(m_pModel);

	m_eDir = static_cast<CDiscord*>(pObj)->Get_Dir_Melee();

	if (DIR_STATE::L == m_eDir)
	{
		m_iAnimnum = 7;
		m_pModel->Set_Animation(7, false);
	}
	else if (DIR_STATE::R == m_eDir)
	{
		m_iAnimnum = 10;
		m_pModel->Set_Animation(10, false);
	}

	else if (DIR_STATE::FL == m_eDir)
	{
		m_iAnimnum = 13;
		m_pModel->Set_Animation(13, false);
	}

	else if (DIR_STATE::FR == m_eDir)
	{
		m_iAnimnum = 16;
		m_pModel->Set_Animation(16, false);
	}

	
}

void CMonster_Pattern_Melee::Update(CGameObject* pObj, float fTimeDelta)
{
	if (ATTACK::ATTACK_IN == m_eAttackState)
	{
		if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
		{
			++m_iAnimnum;
			m_pModel->Set_Animation(m_iAnimnum, true);
			m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
			m_eAttackState = ATTACK::ATTACK_LOOP;
		}
	}
	else if (ATTACK::ATTACK_LOOP == m_eAttackState)
	{
		m_fDuration -= fTimeDelta;

		if (m_fDuration < 0.f)
		{
			++m_iAnimnum;
			m_pModel->Set_Animation(m_iAnimnum, false);
			m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
			m_eAttackState = ATTACK::ATTACK_OUT;
		}
	}
}

void CMonster_Pattern_Melee::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CMonster_Pattern_Melee::Check_Transition(CGameObject* pObj)
{
	if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished() &&
		ATTACK::ATTACK_OUT == m_eAttackState)
	{

		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
		return new CMonster_State_Idle;
	}

	return nullptr;
}
