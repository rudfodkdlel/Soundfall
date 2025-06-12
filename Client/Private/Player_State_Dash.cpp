#include "Player_State_Dash.h"
#include "Player_State_Idle.h"

void CPlayer_State_Dash::Enter(CGameObject* pObj)
{
	__super::Enter(pObj);

	

	m_pPlayer->Move_Pos(m_vDir, 0.001f);

	switch (m_pPlayer->Calc_Dir(m_vDir))
	{
	case Client::NONE:
		m_pModel->Set_Animation(1, false);
		break;
	case Client::F:
		m_pModel->Set_Animation(1, false);
		break;
	case Client::B:
		m_pModel->Set_Animation(0, false);
		break;
	case Client::L:
		m_pModel->Set_Animation(2, false);
		break;
	case Client::R:
		m_pModel->Set_Animation(3, false);
		break;
	case Client::FL:
		m_pModel->Set_Animation(1, false);
		break;
	case Client::FR:
		m_pModel->Set_Animation(1, false);
		break;
	case Client::BL:
		m_pModel->Set_Animation(0, false);
		break;
	case Client::BR:
		m_pModel->Set_Animation(0, false);
		break;
	default:
		break;
	}
	
	if (m_pPlayer->Calc_Dir(m_vDir) == NONE)
	{
		m_vDir = m_pPlayer->Get_Transform()->Get_State(STATE::LOOK);
	}

	m_vDir = XMVector3Normalize(m_vDir);

	m_pPlayer->Toggle_Collider(false);
}

void CPlayer_State_Dash::Update(CGameObject* pObj, float fTimeDelta)
{
	_vector vPos = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);
	vPos += m_vDir * fTimeDelta * 20;

	m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, vPos);


	m_IsFinish = m_pModel->Play_Animation(fTimeDelta * 2.5f);
}

void CPlayer_State_Dash::Exit(CGameObject* pObj)
{
	__super::Exit(pObj);

	m_pPlayer->Toggle_Collider(true);
}

CObject_State* CPlayer_State_Dash::Check_Transition(CGameObject* pObj)
{
	if (m_IsFinish)
	{
		return new CPlayer_State_Idle;
	}

	return nullptr;
}
