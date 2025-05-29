#include "Player_State_Attack.h"
#include "Player_State_Idle.h"
#include "Player.h"
#include "Model.h"
#include "Player_State_Move.h"
#include "Player_State_Dash.h"
#include "Observer_Animation.h"


void CPlayer_State_Attack::Enter(CGameObject* pObj, OBJTYPE eType)
{
	__super::Enter(pObj, eType);
	m_pModel = static_cast<CPlayer*>(pObj)->Get_BodyModel();
	Safe_AddRef(m_pModel);
	m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();

	switch (static_cast<CPlayer*>(pObj)->Get_WeaponType())
	{
	case Client::WEAPON::ASSAULT:
		m_pModel->Set_Animation(24, false);
		break;
	case Client::WEAPON::BAZOOKA:
		m_pModel->Set_Animation(25, false);
		break;
	case Client::WEAPON::GUITAR:
		m_pModel->Set_Animation(26, false);
		break;
	case Client::WEAPON::KEYBOARD:
		m_pModel->Set_Animation(27, false);
		break;

	default:
		break;
	}
}

void CPlayer_State_Attack::Update(CGameObject* pObj, float fTimeDelta)
{
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

	if (0.f < XMVectorGetX(XMVector4Length(m_vDir)))
		m_vDir = XMVector3Normalize(m_vDir);

	

	_vector vPos = pObj->Get_Transform()->Get_State(STATE::POSITION);
	vPos += m_vDir * fTimeDelta * 10.f;

	pObj->Get_Transform()->Set_State(STATE::POSITION, vPos);

	m_vDir = { 0.f, 0.f, 0.f,0.f };

	if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player")))->IsAnimationFinished())
	{
		switch (static_cast<CPlayer*>(pObj)->Get_WeaponType())
		{
		case Client::WEAPON::ASSAULT:
			m_pModel->Set_Animation(24, false);
			break;
		case Client::WEAPON::BAZOOKA:
			m_pModel->Set_Animation(25, false);
			break;
		case Client::WEAPON::GUITAR:
			m_pModel->Set_Animation(26, false);
			break;
		case Client::WEAPON::KEYBOARD:
			m_pModel->Set_Animation(27, false);
			break;

		default:
			break;
		}

		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
	}
	

	
}

void CPlayer_State_Attack::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CPlayer_State_Attack::Check_Transition(CGameObject* pObj)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(pObj);
	if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
		return new CPlayer_State_Dash;
	}

	if (m_pGameInstance->Is_NoKeyPressed())
	{
		return new CPlayer_State_Idle;
	}
	return nullptr;
}
