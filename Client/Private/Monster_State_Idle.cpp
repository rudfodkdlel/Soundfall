#include "Monster_State_Idle.h"
#include "Monster_Pattern_EchoSlam.h"
#include "Monster_Pattern_Melee.h"
#include "Monster_Pattern_Missile.h"
#include "Monster_Pattern_Summon.h"
#include "Monster_Pattern_Wall.h"
#include "Monster_State_Groggy.h"
#include "Monster_State_Death.h"

void CMonster_State_Idle::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);

    int m_iPhase = m_pDiscord->Get_Phase();


    if (0 == m_iPhase)
        m_pModel->Set_Animation(30, false);
    else if (1 == m_iPhase)
        m_pModel->Set_Animation(31, false);
    else if (2 == m_iPhase)
        m_pModel->Set_Animation(32, false);


    m_fSpeedRatio = 1 + m_iPhase * 0.1f;
}

void CMonster_State_Idle::Update(CGameObject* pObj, float fTimeDelta)
{


    m_IsFinish = m_pModel->Play_Animation(fTimeDelta * m_fSpeedRatio);
}

void CMonster_State_Idle::Exit(CGameObject* pObj)
{
    __super::Exit(pObj);
}

CObject_State* CMonster_State_Idle::Check_Transition(CGameObject* pObj)
{
    if (m_pDiscord->Get_CombatCom()->Get_Current_HP() <= 0)
    {
        m_pDiscord->Set_bUseSummon(false);
        auto list = m_pGameInstance->GetLayerList(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Monster_Wall"));
        if (nullptr != list)
        {
            for (auto& object : *list)
            {
                if (object != nullptr)
                    object->Set_Dead();
            }
        }

        return new CMonster_State_Groggy;
    }

    if (m_pDiscord->Check_Groggy())
    {
        m_pDiscord->Set_bUseSummon(false);
        auto list = m_pGameInstance->GetLayerList(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Monster_Wall"));
        if (nullptr != list)
        {
            for (auto& object : *list)
            {
                if (object != nullptr)
                    object->Set_Dead();
            }
        }

        return new CMonster_State_Groggy;
    }

    if (m_IsFinish)
    {
        
        // return new CMonster_State_Groggy;
        
        //m_pDiscord->Get_Next_Skill()
        switch (m_pDiscord->Get_Next_Skill())
        {
        case Client::BP_MELEE:
             return new CMonster_Pattern_Melee;
            break;
        case Client::BP_ECHOSLAM:
            return new CMonster_Pattern_EchoSlam;
            break;
        case Client::BP_SUMMON:
            m_pDiscord->Set_bUseSummon(true);
            return new CMonster_Pattern_Summon;
            break;
        case Client::BP_WALL:
            return new CMonster_Pattern_Wall;
            break;
        case Client::BP_MISSILE:
            return new CMonster_Pattern_Missile;
            break;
        default:
            break;
        }
    }



    return nullptr;
}
