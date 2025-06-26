#include "Monster_Pattern_Missile.h"
#include "Monster_State_Idle.h"
#include "Projectile_Artillery.h"
#include "Monster_State_Groggy.h"

void CMonster_Pattern_Missile::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);
    m_pModel->Set_Animation(4, false);
}

void CMonster_Pattern_Missile::Update(CGameObject* pObj, float fTimeDelta)
{
	if (ATTACK::ATTACK_IN == m_eAttackState)
	{
		if (m_IsFinish)
		{
			m_pModel->Set_Animation(5, true);
			
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
			vPos += {m_pGameInstance->Compute_Random_Normal() * 20 - 10, 10.1f, -m_pGameInstance->Compute_Random_Normal() * 20, 0.f};
			eDesc.vColor = { 1.f,0.f,0.f,1.f };
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
			m_eAttackState = ATTACK::ATTACK_OUT;
		}
	}

	m_IsFinish = m_pModel->Play_Animation(fTimeDelta);
}

void CMonster_Pattern_Missile::Exit(CGameObject* pObj)
{
    __super::Exit(pObj);
}

CObject_State* CMonster_Pattern_Missile::Check_Transition(CGameObject* pObj)
{
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

	if (m_IsFinish && ATTACK::ATTACK_OUT == m_eAttackState)
	{
		return new CMonster_State_Idle;
	}

    return nullptr;
}
