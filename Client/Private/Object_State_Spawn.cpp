#include "Object_State_Spawn.h"
#include "Player.h"
#include "Monster_Base.h"
#include "Model.h"
#include "Player_State_Idle.h"


void CObject_State_Spawn::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);


	if (m_eType == OBJTYPE::PLAYER)
	{
		m_pModel = static_cast<CPlayer*>(pObj)->Get_BodyModel();
	
		Safe_AddRef(m_pModel);
		m_pModel->Set_Animation(11, false);
	}
	else if (m_eType == OBJTYPE::MOSNTER)
	{
		
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
	else if (m_eType == OBJTYPE::MOSNTER)
	{
		
	}

	return nullptr;
}
