#include "Monster_State_Spawn.h"
#include "Monster_State_Idle.h"

void CMonster_State_Spawn::Enter(CGameObject* pObj)
{
	__super::Enter(pObj);

	m_pModel->Set_Animation(33, false);

	m_pGameInstance->StopSound(SOUND_BOSS);
	m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Discord_Spawn_VO.wav"), SOUND_BOSS, 1.f);
}

void CMonster_State_Spawn::Update(CGameObject* pObj, float fTimeDelta)
{
	m_IsFinish = m_pModel->Play_Animation(fTimeDelta);
}

void CMonster_State_Spawn::Exit(CGameObject* pObj)
{
	__super::Exit(pObj);
}

CObject_State* CMonster_State_Spawn::Check_Transition(CGameObject* pObj)
{
	if (m_IsFinish)
	{
		return new CMonster_State_Idle;
	}

	return nullptr;
}
