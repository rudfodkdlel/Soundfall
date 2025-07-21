#include "Player_State_Spawn.h"
#include "Player_State_Idle.h"

void CPlayer_State_Spawn::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);

    m_pModel->Set_Animation(11, false);

    m_pGameInstance->StopSound(SOUND_PLAYER_EFFECT);
    m_pGameInstance->PlaySound(TEXT("Player_Spawn_Start.wav"), SOUND_PLAYER_EFFECT, 0.6f);
}

void CPlayer_State_Spawn::Update(CGameObject* pObj, float fTimeDelta)
{
    m_IsFinish = m_pModel->Play_Animation(fTimeDelta);
}

void CPlayer_State_Spawn::Exit(CGameObject* pObj)
{
    __super::Exit(pObj);
}

CObject_State* CPlayer_State_Spawn::Check_Transition(CGameObject* pObj)
{
    if (m_IsFinish)
    {
        return new CPlayer_State_Idle;
    }
    return nullptr;
}
