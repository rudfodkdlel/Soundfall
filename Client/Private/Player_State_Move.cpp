#include "Player_State_Move.h"
#include "Player_State_Idle.h"
#include "Player_State_Dash.h"
#include "Player_State_Attack_Range.h"
#include "Player_State_Hit.h"

void CPlayer_State_Move::Enter(CGameObject* pObj)
{
	__super::Enter(pObj);
}

void CPlayer_State_Move::Update(CGameObject* pObj, float fTimeDelta)
{
	m_pPlayer->Move_Pos(m_vDir, fTimeDelta);

	
	m_pModel->Set_Animation(m_pPlayer->Get_Range_Weapon()->Get_DirMap()[m_pPlayer->Calc_Dir(m_vDir)], true);
	

	m_pModel->Play_Animation(fTimeDelta);

	m_vDir = {};
}

void CPlayer_State_Move::Exit(CGameObject* pObj)
{
	
	__super::Exit(pObj);
}

CObject_State* CPlayer_State_Move::Check_Transition(CGameObject* pObj)
{
	if (m_pPlayer->Get_CombatCom()->Get_bInvinsible())
	{
		return new CPlayer_State_Hit;
	}


	if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{

		return new CPlayer_State_Dash;
	}

	if (m_pGameInstance->Get_DIMouseState(DIM::LBUTTON) & 0x80)
	{

		return new CPlayer_State_Attack_Range;
	}


	if (!m_pPlayer->Check_Move())
	{
		return new CPlayer_State_Idle;
	}



	return nullptr;
}
