#include "Object_State_Spawn.h"
#include "Player.h"
#include "Monster_Base.h"
#include "Model.h"
#include "Player_State_Idle.h"
#include "Monster_State_Idle.h"


void CObject_State_Spawn::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);

	Safe_AddRef(m_pModel);
	if (m_eType == OBJTYPE::PLAYER)
	{
		m_pModel = static_cast<CPlayer*>(pObj)->Get_BodyModel();
	
		
		m_pModel->Set_Animation(11, false);
	}
	else if (m_eType == OBJTYPE::BOSS)
	{
		m_pModel = static_cast<CMonster_Base*>(pObj)->Get_BodyModel();
		m_pModel->Set_Animation(33, false);
	}
}

void CObject_State_Spawn::Update(CGameObject* pObj, float fTimeDelta)
{
	
	

}

void CObject_State_Spawn::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CObject_State_Spawn::Check_Transition(CGameObject* pObj)
{
	if (m_eType == OBJTYPE::PLAYER)
	{
		if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player")))->IsAnimationFinished())
		{
			m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
			return new CPlayer_State_Idle;
		}

	}
	else if (m_eType == OBJTYPE::BOSS)
	{
		if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
		{
			m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
			return new CMonster_State_Idle;
		}
	}

	return nullptr;
}
