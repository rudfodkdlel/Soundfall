#include "Monster_State_Base.h"
#include "GameInstance.h"

void CMonster_State_Base::Enter(CGameObject* pObj)
{
	m_pGameInstance = CGameInstance::Get_Instance(); Safe_AddRef(m_pGameInstance);
	m_pDiscord = static_cast<CDiscord*>(pObj);
	m_pModel = m_pDiscord->Get_BodyModel();

}

void CMonster_State_Base::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
}
