#include "Player_State_Idle.h"
#include "Player_State_Move.h"
#include "Player_State_Dash.h"
#include "Player_State_Attack_Range.h"
#include "Player_State_Hit.h"
#include "Player_State_Attack_Charge.h"
#include "Player_State_Attack_Combo.h"


void CPlayer_State_Idle::Enter(CGameObject* pObj)
{
	__super::Enter(pObj);

}

void CPlayer_State_Idle::Update(CGameObject* pObj, float fTimeDelta)
{
 	switch (m_pPlayer->Get_Range_Weapon()->Get_WeaponType())
	{
	case WEAPON::ASSAULT:
		m_pModel->Set_Animation(4, true);
		break;
	case WEAPON::KEYBOARD:
		m_pModel->Set_Animation(29, true);
		break;
	case WEAPON::BAZOOKA:
		m_pModel->Set_Animation(47, true);
		break;
	case WEAPON::GUITAR:
		m_pModel->Set_Animation(49, true);
		break;
	default:
		break;
	}

	m_pModel->Play_Animation(fTimeDelta);

}


void CPlayer_State_Idle::Exit(CGameObject* pObj)
{
	__super::Exit(pObj);
}

CObject_State* CPlayer_State_Idle::Check_Transition(CGameObject* pObj)
{

	if (m_pPlayer->Get_CombatCom()->Get_bInvinsible())
	{
		return new CPlayer_State_Hit;
	}

	if (m_pGameInstance->Is_NoKeyPressed())
	{
		return nullptr;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{

		return new CPlayer_State_Dash;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		if (WEAPON::AXE == m_pPlayer->Get_Melee_Weapon()->Get_WeaponType())
		{
			return new CPlayer_State_Attack_Charge;
		}
		
		if (WEAPON::SICKLE == m_pPlayer->Get_Melee_Weapon()->Get_WeaponType())
		{
			return new CPlayer_State_Attack_Combo;
		}
	}

	if (m_pGameInstance->Get_DIMouseState(DIM::LBUTTON) & 0x80)
	{

		return new CPlayer_State_Attack_Range;
	}

	if (m_pPlayer->Check_Move())
	{
		return new CPlayer_State_Move;
	}
	
	return nullptr;
}
