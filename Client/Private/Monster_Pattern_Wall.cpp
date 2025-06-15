#include "Monster_Pattern_Wall.h"
#include "Monster_State_Idle.h"

void CMonster_Pattern_Wall::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);
    m_pModel->Set_Animation(0, false);
}

void CMonster_Pattern_Wall::Update(CGameObject* pObj, float fTimeDelta)
{
	if (ATTACK::ATTACK_IN == m_eAttackState)
	{
		if (m_IsFinish)
		{
			m_pModel->Set_Animation(1, true);
			m_eAttackState = ATTACK::ATTACK_LOOP;

			CGameObject::GAMEOBJECT_DESC eDesc = {};
			_vector vPos = pObj->Get_Transform()->Get_State(STATE::POSITION);

			vPos += {0.f, 0.f, -12.f * m_iCount, 0.f};
			XMStoreFloat4(&eDesc.vPos, vPos);
			m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Discord_Wall"), m_pGameInstance->Get_Current_Level(),
				TEXT("Layer_Boss_Wall"), &eDesc);
			++m_iCount;
		}
	}
	else if (ATTACK::ATTACK_LOOP == m_eAttackState)
	{
		m_fDuration -= fTimeDelta;
		m_fDelay -= fTimeDelta;

		if (m_fDelay < 0.f)
		{
			CGameObject::GAMEOBJECT_DESC eDesc = {};
			_vector vPos = pObj->Get_Transform()->Get_State(STATE::POSITION);

			vPos += {0.f, 0.f, -12.f * m_iCount, 0.f};
			XMStoreFloat4(&eDesc.vPos, vPos);
			m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Discord_Wall"), m_pGameInstance->Get_Current_Level(),
				TEXT("Layer_Monster_Wall"), &eDesc);
			++m_iCount;
			m_fDelay = 0.5f;
		}

		if (m_fDuration < 0.f)
		{
			m_pModel->Set_Animation(2, false);
			m_eAttackState = ATTACK::ATTACK_OUT;
		}
	}

	m_IsFinish = m_pModel->Play_Animation(fTimeDelta);
}

void CMonster_Pattern_Wall::Exit(CGameObject* pObj)
{
    __super::Exit(pObj);
}

CObject_State* CMonster_Pattern_Wall::Check_Transition(CGameObject* pObj)
{
	if (m_IsFinish && ATTACK::ATTACK_OUT == m_eAttackState)
	{

	
		return new CMonster_State_Idle;
	}

    return nullptr;
}
