#include "Player_State_Attack_Charge.h"
#include "Player_State_Idle.h"
#include "Axe.h"
#include "Navigation.h"

void CPlayer_State_Attack_Charge::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);
    
    m_pModel->Set_Animation(15, true);

    m_pPlayer->Get_Range_Weapon()->Set_Active(false);
    m_pPlayer->Get_Melee_Weapon()->Set_Active(true);
    static_cast<CAttack_Area_Player*>(m_pPlayer->Get_PartObjects()[1])->Set_IsActive(true);
    m_pGameInstance->StopSound(SOUND_PLAYER_EFFECT);
    
}

void CPlayer_State_Attack_Charge::Update(CGameObject* pObj, float fTimeDelta)
{

    if (ATTACK::ATTACK_IN == m_eAttackState)
    {
       
        m_pGameInstance->PlaySound(TEXT("Artifact_Axe_Base_Prim_Hold_Loop.wav"), SOUND_PLAYER_EFFECT, 0.6f);
        m_fElapsedTime += fTimeDelta;
        m_fMakeParticleTime -= fTimeDelta;
        if (m_fMakeParticleTime < 0.f)
        {
            m_pPlayer->Get_Melee_Weapon()->Attack(m_vDir);
            m_fMakeParticleTime = 0.5f;
        }
           
      
        if (m_pGameInstance->Key_Up(DIK_SPACE))
        {
            m_fElapsedTime = min(0.5f, m_fElapsedTime);
            m_eAttackState = ATTACK::ATTACK_LOOP;
            m_vDir = m_pPlayer->Get_Transform()->Get_State(STATE::LOOK);
            m_vDir = XMVector3Normalize(m_vDir);
            m_pModel->Set_Animation(17, true);
            static_cast<CAttack_Area_Player*>(m_pPlayer->Get_PartObjects()[1])->Set_IsActive(false);
            m_pGameInstance->StopSound(SOUND_PLAYER_EFFECT);
        }
    }
    else if (ATTACK::ATTACK_LOOP == m_eAttackState)
    {
        if (m_pPlayer->Is_Body_Collision())
        {
            m_eAttackState = ATTACK::ATTACK_OUT;
            m_pModel->Set_Animation(20, false);
            // slash ÀÌÆåÆ®
            return;
        }
        
        m_fElapsedTime -= fTimeDelta;
        if (m_fElapsedTime > 0.f)
        {

            _vector vPos = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);

            vPos += m_vDir * fTimeDelta * 20;

           
            m_pGameInstance->PlaySound(TEXT("Artifact_Axe_Base_Prim_RushForward.wav"), SOUND_PLAYER_EFFECT, 0.6f);
            
            auto pNavi = static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")));

            if (nullptr != pNavi)
            {
                if (pNavi->isMove(vPos))
                    m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, vPos);
            }
            else
            {
                m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, vPos);
            }
            
        }
        else
        {
            m_eAttackState = ATTACK::ATTACK_OUT;
            m_pModel->Set_Animation(20, false);

            m_pGameInstance->StopSound(SOUND_PLAYER_EFFECT);
            m_pGameInstance->PlaySound(TEXT("Artifact_Axe_Base_Prim_WindUp.wav"), SOUND_PLAYER_EFFECT, 0.6f);

            return;
        }
                            
    }

 

    m_IsFinish = m_pModel->Play_Animation(fTimeDelta);

    if ((m_eAttackState == ATTACK::ATTACK_OUT) && (m_pModel->Get_Current_Anim_Ratio() >= 0.35f) && !m_isMakeEffect)
    {
        // slash ÀÌÆåÆ®

        CTrail_Effect_Axe::TRAIL_AXE_DESC eDesc{};

        XMStoreFloat4(&eDesc.vPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

        eDesc.vColor = { 1.f,0.f,1.f,1.f };
        XMStoreFloat4(&eDesc.vDir, XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK)));


        m_pGameInstance->Add_GameObject(static_cast<int>(LEVEL::STATIC), TEXT("Prototype_GameObject_Trail_Effect_Axe"), m_pGameInstance->Get_Current_Level(), TEXT("Layer_Axe_Effect"), &eDesc);

        auto pEffect = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Axe_Effect"));


        m_pTrail = static_cast<CTrail_Effect_Axe*>(pEffect);

        m_isMakeEffect = true;
    }
}

void CPlayer_State_Attack_Charge::Exit(CGameObject* pObj)
{
    __super::Exit(pObj);
   

}

CObject_State* CPlayer_State_Attack_Charge::Check_Transition(CGameObject* pObj)
{
    if (m_IsFinish && ATTACK::ATTACK_OUT == m_eAttackState)
    {
        static_cast<CCollider*>(m_pPlayer->Get_Melee_Weapon()->Get_Component(TEXT("Com_Collider")))->Set_Active(false);
        static_cast<CAxe*>(m_pPlayer->Get_Melee_Weapon())->Reset();
        m_pPlayer->Get_Range_Weapon()->Set_Active(true);
        m_pPlayer->Get_Melee_Weapon()->Set_Active(false);

        m_pTrail->Set_Dead();

        return new CPlayer_State_Idle;
    }

    return nullptr;
}
