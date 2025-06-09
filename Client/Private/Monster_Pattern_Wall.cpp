#include "Monster_Pattern_Wall.h"
#include "Monster_Base.h"
#include "Monster_State_Idle.h"


void CMonster_Pattern_Wall::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);

	m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();


	m_pModel = static_cast<CMonster_Base*>(pObj)->Get_BodyModel();
	Safe_AddRef(m_pModel);

	m_pModel->Set_Animation(0, false);
}

void CMonster_Pattern_Wall::Update(CGameObject* pObj, float fTimeDelta)
{
	if (ATTACK::ATTACK_IN == m_eAttackState)
	{
		if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
		{
			m_pModel->Set_Animation(1, true);
			m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
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
			m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
			m_eAttackState = ATTACK::ATTACK_OUT;
		}
	}
}

void CMonster_Pattern_Wall::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CMonster_Pattern_Wall::Check_Transition(CGameObject* pObj)
{
	if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished() &&
		ATTACK::ATTACK_OUT == m_eAttackState)
	{

		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
		return new CMonster_State_Idle;
	}

	return nullptr;
}
