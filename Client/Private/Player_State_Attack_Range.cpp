#include "Player_State_Attack_Range.h"
#include "Player_State_Dash.h"
#include "Player_State_Idle.h"
#include "Player_State_Hit.h"

void CPlayer_State_Attack_Range::Enter(CGameObject* pObj)
{
	__super::Enter(pObj);

	switch (m_pPlayer->Get_Range_Weapon()->Get_WeaponType())
	{
	case Client::WEAPON::ASSAULT:
		m_pModel->Set_Animation(24, true);
		break;
	case Client::WEAPON::BAZOOKA:
		m_pModel->Set_Animation(25, true);
		break;
	case Client::WEAPON::GUITAR:
		m_pModel->Set_Animation(26, true);
		break;
	case Client::WEAPON::KEYBOARD:
		m_pModel->Set_Animation(28, false);
		break;

	case Client::WEAPON::END:
		break;
	default:
		break;
	}

	m_pPlayer->Get_Range_Weapon()->Check_Timing();
}

void CPlayer_State_Attack_Range::Update(CGameObject* pObj, float fTimeDelta)
{
	m_pPlayer->Move_Pos(m_vDir, fTimeDelta);

	m_vDir = {};

	if (m_IsFinish)
	{
		m_pModel->Set_Animation(30, true);
	}

	if (m_pGameInstance->Key_Pressing(DIM::LBUTTON))
	{
		m_pPlayer->Get_Range_Weapon()->Attack(XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK)));
	}
	
	

	m_IsFinish = m_pModel->Play_Animation(fTimeDelta);
}

void CPlayer_State_Attack_Range::Exit(CGameObject* pObj)
{
	__super::Exit(pObj);
}

CObject_State* CPlayer_State_Attack_Range::Check_Transition(CGameObject* pObj)
{
	if (m_pPlayer->Get_CombatCom()->Get_bInvinsible())
	{
		return new CPlayer_State_Hit;
	}


	if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{

		return new CPlayer_State_Dash;
	}

	if (m_pGameInstance->Key_Up(DIM::LBUTTON))
	{
		return new CPlayer_State_Idle;
	}

	return nullptr;
}
