#include "Monster_State_Death.h"
#include "Monster_State_Idle.h"
#include "Discord.h"

void CMonster_State_Death::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);


	m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();


	m_pModel = static_cast<CMonster_Base*>(pObj)->Get_BodyModel();
	Safe_AddRef(m_pModel);

	m_pModel->Set_Animation(20, false);
}

void CMonster_State_Death::Update(CGameObject* pObj, float fTimeDelta)
{
	auto pObserver = static_cast<CObserver_Animation*>(
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")));

	if (m_eAnimState == ANIMSTATE::ANIM_IN)
	{
		if (pObserver->IsAnimationFinished())
		{
			m_eAnimState = ANIMSTATE::ANIM_LOOP;
			pObserver->Reset();
			m_pModel->Set_Animation(21, true);
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
			m_pModel->Set_Animation(22, false);
		}
	}
}

void CMonster_State_Death::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CMonster_State_Death::Check_Transition(CGameObject* pObj)
{
	auto pObserver = static_cast<CObserver_Animation*>(
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")));

	if (ANIMSTATE::ANIM_OUT == m_eAnimState && pObserver->IsAnimationFinished())
	{
		pObj->Set_Dead();
	}
    return nullptr;
}
