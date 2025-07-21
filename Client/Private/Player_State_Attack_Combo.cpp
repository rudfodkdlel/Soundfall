#include "Player_State_Attack_Combo.h"
#include "Player_State_Idle.h"
#include "Sickle.h"

void CPlayer_State_Attack_Combo::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);

    m_pModel->Set_Animation(5, false);
    m_pPlayer->Get_Range_Weapon()->Set_Active(false);
    m_pPlayer->Get_Melee_Weapon()->Set_Active(true);
    static_cast<CCollider*>(m_pPlayer->Get_Melee_Weapon()->Get_Component(TEXT("Com_Collider")))->Set_Active(true);
    m_pGameInstance->StopSound(SOUND_PLAYER_EFFECT);
    m_pGameInstance->PlaySound(TEXT("Artifact_Scythe_Base_Whoosh_0.wav"), SOUND_PLAYER_EFFECT, 0.6f);

}

void CPlayer_State_Attack_Combo::Update(CGameObject* pObj, float fTimeDelta)
{
    m_IsFinish = m_pModel->Play_Animation(fTimeDelta);

    if (m_eAttackState == ATTACK::ATTACK_OUT ||
        m_pModel->Get_Current_Anim_Ratio() <= 0.3f ||
        m_IsInput)
    {
        return;
    }


    if (m_pGameInstance->Key_Down(DIK_SPACE))
    {
        m_IsInput = true;
        static_cast<CCollider*>(m_pPlayer->Get_Melee_Weapon()->Get_Component(TEXT("Com_Collider")))->Set_Active(true);
        static_cast<CSickle*>(m_pPlayer->Get_Melee_Weapon())->Reset();
        switch (m_iComboCount)
        {
        case 0:
            m_iComboCount = 1;
            m_pModel->Set_Animation(6, false);
            m_IsInput = false;
            m_pGameInstance->StopSound(SOUND_PLAYER_EFFECT);
            m_pGameInstance->PlaySound(TEXT("Artifact_Scythe_Base_Whoosh_0.wav"), SOUND_PLAYER_EFFECT, 0.6f);
            break;
        case 1:
            if ((m_pGameInstance->Get_Timing() < 0.15f))
            {
                m_pModel->Set_Animation(8, false);
                m_pGameInstance->StopSound(SOUND_PLAYER_EFFECT);   
                m_pGameInstance->PlaySound(TEXT("Artifact_Scythe_Brass_UpperCut.wav"), SOUND_PLAYER_EFFECT, 0.6f);
            }
            else
            {
                m_pModel->Set_Animation(7, false);
                m_pGameInstance->StopSound(SOUND_PLAYER_EFFECT);
                m_pGameInstance->PlaySound(TEXT("Artifact_Scythe_Base_Whoosh_0.wav"), SOUND_PLAYER_EFFECT, 0.6f);
            }
               
            m_iComboCount = 2;
            m_eAttackState = ATTACK::ATTACK_OUT;
            break;
        default:
            break;
        }
    }
   


 
}

void CPlayer_State_Attack_Combo::Exit(CGameObject* pObj)
{
    __super::Exit(pObj);
  
}

CObject_State* CPlayer_State_Attack_Combo::Check_Transition(CGameObject* pObj)
{
    if (m_IsFinish)
    {
        // ATTACK_OUT 상태이거나, 입력이 없을 경우 
        if (m_eAttackState == ATTACK::ATTACK_OUT || !m_IsInput)
        {
            m_pPlayer->Get_Range_Weapon()->Set_Active(true);
            m_pPlayer->Get_Melee_Weapon()->Set_Active(false);
            static_cast<CSickle*>(m_pPlayer->Get_Melee_Weapon())->Reset();
            static_cast<CCollider*>(m_pPlayer->Get_Melee_Weapon()->Get_Component(TEXT("Com_Collider")))->Set_Active(false);
            return new CPlayer_State_Idle;
        }
           
    }

    return nullptr;
}
