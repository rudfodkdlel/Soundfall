#include "Player_State_Idle.h"
#include "Player.h"
#include "Model.h"
#include "Player_State_Move.h"
#include "Player_State_Dash.h"
#include "Player_State_Attack.h"
#include "Player_State_ULT.h"
#include "Player_State_Combo.h"
#include "Player_State_Charge.h"
#include "Player_State_Change.h"

void CPlayer_State_Idle::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);
	m_pModel = static_cast<CPlayer*>(pObj)->Get_BodyModel();

	Safe_AddRef(m_pModel);



}

void CPlayer_State_Idle::Update(CGameObject* pObj, float fTimeDelta)
{
 	switch (static_cast<CPlayer*>(pObj)->Get_Range_Weapon()->Get_WeaponType())
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


}


void CPlayer_State_Idle::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CPlayer_State_Idle::Check_Transition(CGameObject* pObj)
{

	if (m_pGameInstance->Is_NoKeyPressed())
	{
		return nullptr;
	}
	
	if (static_cast<CPlayer*>(pObj)->Check_Move())
	{
		return new CPlayer_State_Move;
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{

		return new CPlayer_State_Dash;
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
	{

		return new CPlayer_State_ULT;
	}
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		if (WEAPON::AXE == static_cast<CPlayer*>(pObj)->Get_Melee_Weapon()->Get_WeaponType())
		{
			return new CPlayer_State_Charge;
		}
		else if (WEAPON::SICKLE == static_cast<CPlayer*>(pObj)->Get_Melee_Weapon()->Get_WeaponType())
		{
			return new CPlayer_State_Combo;
		}
		
	}
	if (m_pGameInstance->Get_DIMouseState(DIM::LBUTTON) & 0x80)
	{

		return new CPlayer_State_Attack;
	}
	
	if ((m_pGameInstance->Get_DIKeyState(DIK_1) & 0x80) || (m_pGameInstance->Get_DIKeyState(DIK_2) & 0x80))
	{
		return new CPlayer_State_Change;
	}

		
	
	return nullptr;
}
