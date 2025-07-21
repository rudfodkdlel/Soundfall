#include "Monster_Pattern_Melee.h"
#include "Monster_State_Idle.h"
#include "Body_Discord.h"
#include "Monster_State_Groggy.h"

void CMonster_Pattern_Melee::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);

    m_eDir = m_pDiscord->Get_Dir_Melee();

	auto colliders = static_cast<CBody_Discord*>(static_cast<CDiscord*>(pObj)->Get_Body())->Get_Colliders();

	static_cast<CDiscord*>(pObj)->Get_CombatCom()->Set_Damage(20);


	if (DIR_STATE::L == m_eDir)
	{
		m_iAnimnum = 7;
		m_pModel->Set_Animation(7, false);
		//colliders[1]->Set_Active(true);
	}
	else if (DIR_STATE::R == m_eDir)
	{
		m_iAnimnum = 10;
		m_pModel->Set_Animation(10, false);
		//colliders[4]->Set_Active(true);
	}

	else if (DIR_STATE::FL == m_eDir)
	{
		m_iAnimnum = 13;
		m_pModel->Set_Animation(13, false);
		//colliders[2]->Set_Active(true);
	}

	else if (DIR_STATE::FR == m_eDir)
	{
		m_iAnimnum = 16;
		m_pModel->Set_Animation(16, false);
		//colliders[3]->Set_Active(true);
	}
}

void CMonster_Pattern_Melee::Update(CGameObject* pObj, float fTimeDelta)
{
	if (ATTACK::ATTACK_IN == m_eAttackState)
	{
		if (m_pModel->Get_Current_Anim_Ratio() > 0.7f)
		{
			auto colliders = static_cast<CBody_Discord*>(static_cast<CDiscord*>(pObj)->Get_Body())->Get_Colliders();
			
			if (!m_pDiscord->Is_Attack_Hit())
			{
				int idx = -1;
				switch (m_eDir)
				{
				case DIR_STATE::L:  idx = 1; break;
				case DIR_STATE::FL: idx = 2; break;
				case DIR_STATE::FR: idx = 3; break;
				case DIR_STATE::R:  idx = 4; break;
				}

				if (idx != -1)
				{
					if (!colliders[idx]->Get_Active())
						colliders[idx]->Set_Active(true);
				}

				m_pGameInstance->StopSound(SOUND_BOSS);
				m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Discord_Slam_Ground_0.wav"), SOUND_BOSS, 1.f);
			}
			
		}

		if (m_IsFinish)
		{
			++m_iAnimnum;
			m_pModel->Set_Animation(m_iAnimnum, true);
			m_eAttackState = ATTACK::ATTACK_LOOP;
		}
	}
	else if (ATTACK::ATTACK_LOOP == m_eAttackState)
	{
		m_fDuration -= fTimeDelta;

		if (m_fDuration < 0.f)
		{
			++m_iAnimnum;
			m_pModel->Set_Animation(m_iAnimnum, false);
			m_eAttackState = ATTACK::ATTACK_OUT;
		}
	}

	m_IsFinish =m_pModel->Play_Animation(fTimeDelta);
}

void CMonster_Pattern_Melee::Exit(CGameObject* pObj)
{
	__super::Exit(pObj);
	
}

CObject_State* CMonster_Pattern_Melee::Check_Transition(CGameObject* pObj)
{
	if (m_pDiscord->Check_Groggy())
	{
		m_pDiscord->Reset_Attack_Hit();
		auto colliders = static_cast<CBody_Discord*>(m_pDiscord->Get_Body())->Get_Colliders();

		for (int i = 0; i < 5; ++i)
			colliders[i]->Set_Active(false);

		static_cast<CDiscord*>(pObj)->Get_CombatCom()->Set_Damage(0);

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
		m_pDiscord->Reset_Attack_Hit();
		auto colliders = static_cast<CBody_Discord*>(m_pDiscord->Get_Body())->Get_Colliders();

		for (int i = 0; i < 5; ++i)
			colliders[i]->Set_Active(false);

		static_cast<CDiscord*>(pObj)->Get_CombatCom()->Set_Damage(0);
	

		return new CMonster_State_Idle;
	}

	return nullptr;
}
