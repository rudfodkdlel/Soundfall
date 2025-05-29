#include "Player_State_ULT.h"
#include "Player_State_Idle.h"
#include "Player.h"
#include "Model.h"
#include "Player_State_Move.h"
#include "Player_State_Dash.h"
#include "Player_State_Change.h"

void CPlayer_State_ULT::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);
	m_pModel = static_cast<CPlayer*>(pObj)->Get_BodyModel();
	
	m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();

	static_cast<CPlayer*>(pObj)->Get_Range_Weapon()->Set_Active(false);
	static_cast<CPlayer*>(pObj)->Get_Melee_Weapon()->Set_Active(true);

	m_eWeaponType = static_cast<CPlayer*>(pObj)->Get_Melee_Weapon()->Get_WeaponType();

	if (WEAPON::SICKLE == m_eWeaponType)
	{
		m_pModel->Set_Animation(12, false);
	}
	else if (WEAPON::AXE == m_eWeaponType)
	{
		m_pModel->Set_Animation(22, true);
	}
}

void CPlayer_State_ULT::Update(CGameObject* pObj, float fTimeDelta)
{
	if (WEAPON::SICKLE == m_eWeaponType)
	{
		if (ATTACK::ATTACK_IN == m_eAttackState)
		{
			if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player")))->IsAnimationFinished())
			{
				m_pModel->Set_Animation(13, true);
				m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
				m_eAttackState = ATTACK::ATTACK_LOOP;
			}
		}
		else if (ATTACK::ATTACK_LOOP == m_eAttackState)
		{
			m_fDuration -= fTimeDelta;

			if (m_fDuration < 0.f)
			{
				m_pModel->Set_Animation(14, false);
				m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
				m_eAttackState = ATTACK::ATTACK_OUT;
			}
		}
	}
	else if (WEAPON::AXE == m_eWeaponType)
	{
		if (m_pGameInstance->Get_DIMouseState(DIM::LBUTTON) & 0x80)
		{
			m_pModel->Set_Animation(23, false);
			m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
			m_eAttackState = ATTACK::ATTACK_OUT;
		}
	}


}

void CPlayer_State_ULT::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	
	static_cast<CPlayer*>(pObj)->Get_Range_Weapon()->Set_Active(true);
	static_cast<CPlayer*>(pObj)->Get_Melee_Weapon()->Set_Active(false);
}

CObject_State* CPlayer_State_ULT::Check_Transition(CGameObject* pObj)
{
	if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player")))->IsAnimationFinished() &&
		ATTACK::ATTACK_OUT == m_eAttackState)
	{
	
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
		return new CPlayer_State_Change;
	}
	
	return nullptr;
}