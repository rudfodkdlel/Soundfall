#include "Player_State_Dash.h"
#include "Player.h"
#include "Model.h"
#include "Player_State_Idle.h"
#include "CombatStat.h"


void CPlayer_State_Dash::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);

	m_pModel = static_cast<CPlayer*>(pObj)->Get_BodyModel();
	Safe_AddRef(m_pModel);

	static_cast<CCombatStat*>(pObj->Get_Component(TEXT("Com_Combat")))->Set_bInvinsible(true);

	m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();

	if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_vDir += {-1.f, 0.f, 0.f, 0.f};

	}
	if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_vDir += {1.f, 0.f, 0.f, 0.f};

	}
	if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_vDir += {0.f, 0.f, 1.f, 0.f};

	}
	if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_vDir += {0.f, 0.f, -1.f, 0.f};
	}

	// 아무것도 안눌렀으면 현재 보는 방향으로
	if (0.f == XMVectorGetX(XMVector4Length(m_vDir)))
		m_vDir += pObj->Get_Transform()->Get_State(STATE::LOOK);
	
	m_vDir = XMVector3Normalize(m_vDir);

	DIR_STATE eDirState = static_cast<CPlayer*>(pObj)->Calc_Dir(m_vDir);

	switch (eDirState)
	{
	case F:
		m_pModel->Set_Animation(1, false);
		break;
	case B:
		m_pModel->Set_Animation(0, false);
		break;
	case L:
		m_pModel->Set_Animation(2, false);
		break;
	case R:
		m_pModel->Set_Animation(3, false);
		break;
	case FL:
		m_pModel->Set_Animation(1, false);
		break;
	case FR:
		m_pModel->Set_Animation(1, false);
		break;
	case BL:
		m_pModel->Set_Animation(0, false);
		break;
	case BR:
		m_pModel->Set_Animation(0, false);
		break;

	default:
		break;
	}
}

void CPlayer_State_Dash::Update(CGameObject* pObj, float fTimeDelta)
{
	
	_vector vPos = pObj->Get_Transform()->Get_State(STATE::POSITION);
	vPos += m_vDir * fTimeDelta * 12.f;

	pObj->Get_Transform()->Set_State(STATE::POSITION, vPos);
}



void CPlayer_State_Dash::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
	
	
}

CObject_State* CPlayer_State_Dash::Check_Transition(CGameObject* pObj)
{
	if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player")))->IsAnimationFinished())
	{
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
		static_cast<CCombatStat*>(pObj->Get_Component(TEXT("Com_Combat")))->Set_bInvinsible(false);
		return new CPlayer_State_Idle;
	}

	return nullptr;
}
