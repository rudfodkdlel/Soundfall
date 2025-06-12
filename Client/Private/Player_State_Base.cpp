#include "Player_State_Base.h"
#include "GameInstance.h"

void CPlayer_State_Base::Enter(CGameObject* pObj)
{
	m_pGameInstance = CGameInstance::Get_Instance(); Safe_AddRef(m_pGameInstance);
	m_pPlayer = static_cast<CPlayer*>(pObj);
	m_pModel = m_pPlayer->Get_BodyModel();

}

void CPlayer_State_Base::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
}
