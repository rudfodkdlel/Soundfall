#include "Player_State_Move.h"
#include "Player.h"
#include "Model.h"
#include "Player_State_Idle.h"
#include "Player_State_Dash.h"
#include "Player_State_Attack.h"


void CPlayer_State_Move::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);
	m_pModel = static_cast<CPlayer*>(pObj)->Get_BodyModel();
	Safe_AddRef(m_pModel);

}

void CPlayer_State_Move::Update(CGameObject* pObj, float fTimeDelta)
{
	static_cast<CPlayer*>(pObj)->Move_Pos(m_vDir, fTimeDelta);

	DIR_STATE eDirState = static_cast<CPlayer*>(pObj)->Calc_Dir(m_vDir);
	CWeapon_Base* pWeapon = static_cast<CPlayer*>(pObj)->Get_Range_Weapon();

	m_pModel->Set_Animation(pWeapon->Get_DirMap()[eDirState], true);

	m_vDir = { 0.f,0.f,0.f,0.f };

}
	

void CPlayer_State_Move::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CPlayer_State_Move::Check_Transition(CGameObject* pObj)
{
	
	if (m_pGameInstance->Get_DIMouseState(DIM::LBUTTON) & 0x80)
	{
		
		return new CPlayer_State_Attack;
	}
	else if (m_pGameInstance->Is_NoKeyPressed())
	{
		return new CPlayer_State_Idle;
	}
	else if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{
		
		return new CPlayer_State_Dash;
	}

	
	return nullptr;
}
