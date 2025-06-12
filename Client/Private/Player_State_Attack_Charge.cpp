#include "Player_State_Attack_Charge.h"
#include "Player_State_Idle.h"
#include "Axe.h"

void CPlayer_State_Attack_Charge::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);
    
    m_pModel->Set_Animation(15, true);

    m_pPlayer->Get_Range_Weapon()->Set_Active(false);
    m_pPlayer->Get_Melee_Weapon()->Set_Active(true);
}

void CPlayer_State_Attack_Charge::Update(CGameObject* pObj, float fTimeDelta)
{

    if (ATTACK::ATTACK_IN == m_eAttackState)
    {
        m_fElapsedTime += fTimeDelta;
      
        if (m_pGameInstance->Key_Up(DIK_SPACE))
        {
            m_fElapsedTime = min(1.f, m_fElapsedTime);
            m_eAttackState = ATTACK::ATTACK_LOOP;
            m_vDir = m_pPlayer->Get_Transform()->Get_State(STATE::LOOK);
            m_vDir = XMVector3Normalize(m_vDir);
            m_pModel->Set_Animation(17, true);
        }
    }
    else if (ATTACK::ATTACK_LOOP == m_eAttackState)
    {
        if (m_pPlayer->Is_Body_Collision())
        {
            m_eAttackState = ATTACK::ATTACK_OUT;
            m_pModel->Set_Animation(20, false);
            static_cast<CCollider*>(m_pPlayer->Get_Melee_Weapon()->Get_Component(TEXT("Com_Collider")))->Set_Active(true);
            return;
        }
        
        m_fElapsedTime -= fTimeDelta;
        if (m_fElapsedTime > 0.f)
        {

            _vector vPos = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);

            vPos += m_vDir * fTimeDelta * 20;

            m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, vPos);
        }
        else
        {
            m_eAttackState = ATTACK::ATTACK_OUT;
            m_pModel->Set_Animation(20, false);
            return;
        }
       
    }

    m_IsFinish = m_pModel->Play_Animation(fTimeDelta);
}

void CPlayer_State_Attack_Charge::Exit(CGameObject* pObj)
{
    __super::Exit(pObj);
    static_cast<CCollider*>(m_pPlayer->Get_Melee_Weapon()->Get_Component(TEXT("Com_Collider")))->Set_Active(false);
    static_cast<CAxe*>(m_pPlayer->Get_Melee_Weapon())->Reset();
    m_pPlayer->Get_Range_Weapon()->Set_Active(true);
    m_pPlayer->Get_Melee_Weapon()->Set_Active(false);

}

CObject_State* CPlayer_State_Attack_Charge::Check_Transition(CGameObject* pObj)
{
    if (m_IsFinish && ATTACK::ATTACK_OUT == m_eAttackState)
    {
        return new CPlayer_State_Idle;
    }

    return nullptr;
}
