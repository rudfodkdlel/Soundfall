#include "Monster_Pattern_Missile.h"
#include "Monster_Base.h"
#include "Monster_State_Idle.h"
#include "Projectile_Artillery.h"

void CMonster_Pattern_Missile::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);

	m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();


	m_pModel = static_cast<CMonster_Base*>(pObj)->Get_BodyModel();
	Safe_AddRef(m_pModel);

	m_pModel->Set_Animation(4, false);
}

void CMonster_Pattern_Missile::Update(CGameObject* pObj, float fTimeDelta)
{
	if (ATTACK::ATTACK_IN == m_eAttackState)
	{
		if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
		{
			m_pModel->Set_Animation(5, true);
			m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
			m_eAttackState = ATTACK::ATTACK_LOOP;
		}
	}
	else if (ATTACK::ATTACK_LOOP == m_eAttackState)
	{
		m_fDuration -= fTimeDelta;
		m_fDelay -= fTimeDelta;
		if (m_fDelay < 0.f)
		{
			CProjectile_Artillery::PROJECTILE_DESC eDesc = {};
			eDesc.fSpeedPerSec = 1.f;

			_vector vDir = XMVector3Normalize(pObj->Get_Transform()->Get_State(STATE::LOOK));
			_vector vPos = pObj->Get_Transform()->Get_State(STATE::POSITION);
			vPos += {m_pGameInstance->Compute_Random_Normal() * 20 - 10, 0.f, -m_pGameInstance->Compute_Random_Normal() * 20, 0.f};

			XMStoreFloat4(&eDesc.vPos, vPos);
			XMStoreFloat4(&eDesc.vDir, vDir);
			// 투사체 생성해서 날아가게 해보자
			m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Artillery"), m_pGameInstance->Get_Current_Level(),
				TEXT("Layer_Projectile_Monster"), &eDesc);
			m_fDelay = 0.3f;
		}
		

		if (m_fDuration < 0.f)
		{
			m_pModel->Set_Animation(6, false);
			m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
			m_eAttackState = ATTACK::ATTACK_OUT;
		}
	}
}

void CMonster_Pattern_Missile::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CMonster_Pattern_Missile::Check_Transition(CGameObject* pObj)
{
	if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished() &&
		ATTACK::ATTACK_OUT == m_eAttackState)
	{

		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
		return new CMonster_State_Idle;
	}

	return nullptr;
}
