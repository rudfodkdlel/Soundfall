#include "Monster_Pattern_EchoSlam.h"
#include "Monster_State_Idle.h"
#include "Projectile_Base.h"
#include "Monster_State_Groggy.h"


void CMonster_Pattern_EchoSlam::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);

    m_pModel->Set_Animation(3, false);
}

void CMonster_Pattern_EchoSlam::Update(CGameObject* pObj, float fTimeDelta)
{
	m_fTime -= fTimeDelta;

	if (m_fTime < 0.f)
	{
		if (!m_IsAttack)
		{
			CProjectile_Base::PROJECTILE_DESC eDesc = {};
			eDesc.fSpeedPerSec = 1.f;

			_vector vDir = XMVector3Normalize(pObj->Get_Transform()->Get_State(STATE::LOOK));
			_vector vPos = pObj->Get_Transform()->Get_State(STATE::POSITION);
			vPos += { 0.f, 10.1f, -10.f, 0.f};

			XMStoreFloat4(&eDesc.vPos, vPos);
			XMStoreFloat4(&eDesc.vDir, vDir);

			m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Shockwave"), m_pGameInstance->Get_Current_Level(), TEXT("Layer_Projectile_Monster"), &eDesc);


			m_IsAttack = true;
		}
	}

	m_IsFinish = m_pModel->Play_Animation(fTimeDelta);
}

void CMonster_Pattern_EchoSlam::Exit(CGameObject* pObj)
{
    __super::Exit(pObj);
}

CObject_State* CMonster_Pattern_EchoSlam::Check_Transition(CGameObject* pObj)
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

	if (m_IsFinish)
	{
		return new CMonster_State_Idle;
	}
    return nullptr;
}
