#include "Monster_Pattern_Spawn.h"
#include "Monster_Base.h"
#include "Observer_Animation.h"
#include "Monster_State_Idle.h"

void CMonster_Pattern_Spawn::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);

	m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();


	m_pModel = static_cast<CMonster_Base*>(pObj)->Get_BodyModel();
	Safe_AddRef(m_pModel);

	m_pModel->Set_Animation(19, false);
}

void CMonster_Pattern_Spawn::Update(CGameObject* pObj, float fTimeDelta)
{
	m_fDelay -= fTimeDelta;

	if (m_fDelay < 0.f)
	{
		if (m_iSpawnCount > 0)
		{
			CGameObject::GAMEOBJECT_DESC eDesc = {};

			_vector vPos = pObj->Get_Transform()->Get_State(STATE::POSITION);
			vPos += {m_pGameInstance->Compute_Random_Normal() * 40 - 20, 0.f, -m_pGameInstance->Compute_Random_Normal() * 30, 0.f};

			XMStoreFloat4(&eDesc.vPos, vPos);

			m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Tentacle_Melee"),
				static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Monster"), &eDesc);

			--m_iSpawnCount;
		}

		m_fDelay = 0.3f;
	}

	



}

void CMonster_Pattern_Spawn::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CMonster_Pattern_Spawn::Check_Transition(CGameObject* pObj)
{
	if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
	{
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
		return new CMonster_State_Idle;
	}

	return nullptr;
}
