#include "Monster_Pattern_Wall.h"
#include "Monster_State_Idle.h"
#include "Monster_State_Groggy.h"

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
			_vector basePos = pObj->Get_Transform()->Get_State(STATE::POSITION);

			for (int i = 0; i < 3; ++i)
			{
				_vector vPos = basePos;  // 매 반복마다 초기 위치로 복원
				_vector offset = { float(-20 + 20 * i), 10.f, -13.f * m_iCount, 0.f };
				vPos += offset;

				m_pGameInstance->StopSound(SOUND_MONSTER_DEATH);
				m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Discord_CreateWall_VO_0.wav"), SOUND_MONSTER_DEATH, 1.f);

				XMStoreFloat4(&eDesc.vPos, vPos);
				m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Discord_Wall"), m_pGameInstance->Get_Current_Level(),
					TEXT("Layer_Monster_Wall"), &eDesc);
			}

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
			_vector basePos = pObj->Get_Transform()->Get_State(STATE::POSITION);

			for (int i = 0; i < 3; ++i)
			{
				m_pGameInstance->StopSound(SOUND_MONSTER_DEATH);
				m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Discord_CreateWall_VO_0.wav"), SOUND_MONSTER_DEATH, 1.f);

				_vector vPos = basePos;  // 매 반복마다 초기 위치로 복원
				_vector offset = { float(-20 + 20 * i), 10.f, -13.f * m_iCount, 0.f };
				vPos += offset;

				XMStoreFloat4(&eDesc.vPos, vPos);
				m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Discord_Wall"), m_pGameInstance->Get_Current_Level(),
					TEXT("Layer_Monster_Wall"), &eDesc);
			}

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
