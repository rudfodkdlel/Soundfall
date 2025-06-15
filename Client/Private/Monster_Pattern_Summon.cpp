#include "Monster_Pattern_Summon.h"
#include "Monster_State_Idle.h"

void CMonster_Pattern_Summon::Enter(CGameObject* pObj)
{
	__super::Enter(pObj);

	m_pModel->Set_Animation(19, false);
}

void CMonster_Pattern_Summon::Update(CGameObject* pObj, float fTimeDelta)
{
	m_fDelay -= fTimeDelta;

	if (m_fDelay < 0.f)
	{
		if (m_iSpawnCount > 0)
		{
			CContainerObject::CONTAINEROBJECT_DESC eDesc = {};
			eDesc.iNumPartObjects = ENUM_CLASS(PART_DEFAULT::END);

			_vector vPos = pObj->Get_Transform()->Get_State(STATE::POSITION);
			vPos += {m_pGameInstance->Compute_Random_Normal() * 40 - 20, 0.f, -m_pGameInstance->Compute_Random_Normal() * 30, 0.f};

			XMStoreFloat4(&eDesc.vPos, vPos);

			m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Tentacle_Melee"),
				static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Boss_Spawn"), &eDesc);

			--m_iSpawnCount;
		}

		// 원거리 추가, 원거리는 지정된 위치에 생성

		m_fDelay = 0.3f;
	}

	m_IsFinish = m_pModel->Play_Animation(fTimeDelta);
}

void CMonster_Pattern_Summon::Exit(CGameObject* pObj)
{
	__super::Exit(pObj);
}

CObject_State* CMonster_Pattern_Summon::Check_Transition(CGameObject* pObj)
{
	if (m_IsFinish)
	{
		
		return new CMonster_State_Idle;
	}

	return nullptr;
}
