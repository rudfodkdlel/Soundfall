#include "Monster_State_Groggy.h"

void CMonster_State_Groggy::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);
}

void CMonster_State_Groggy::Update(CGameObject* pObj, float fTimeDelta)
{
    if(m_pDiscord->Get_Body())

    m_pModel->Play_Animation(fTimeDelta);
}

void CMonster_State_Groggy::Exit(CGameObject* pObj)
{
    __super::Exit(pObj);
}

CObject_State* CMonster_State_Groggy::Check_Transition(CGameObject* pObj)
{
    return nullptr;
}
