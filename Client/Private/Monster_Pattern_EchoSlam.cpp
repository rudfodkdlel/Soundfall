#include "Monster_Pattern_EchoSlam.h"
#include "Monster_Base.h"
#include "Observer_Animation.h"
#include "Monster_State_Idle.h"
#include "Projectile_Shockwave.h"

void CMonster_Pattern_EchoSlam::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);

	m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();


	m_pModel = static_cast<CMonster_Base*>(pObj)->Get_BodyModel();
	Safe_AddRef(m_pModel);

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
			vPos += { 0.f, 0.f, -10.f, 0.f};

			XMStoreFloat4(&eDesc.vPos, vPos);
			XMStoreFloat4(&eDesc.vDir, vDir);

			m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Shockwave"), m_pGameInstance->Get_Current_Level(), TEXT("Layer_Projectile_Monster"), &eDesc);


			m_IsAttack = true;
		}
	}
	
}

void CMonster_Pattern_EchoSlam::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CMonster_Pattern_EchoSlam::Check_Transition(CGameObject* pObj)
{
	if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
	{
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord"))->Reset();
		return new CMonster_State_Idle;
	}

	return nullptr;
}
