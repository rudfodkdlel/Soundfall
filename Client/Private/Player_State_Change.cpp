#include "Player_State_Change.h"
#include "Player.h"
#include "Monster_Base.h"
#include "Model.h"
#include "Player_State_Idle.h"


void CPlayer_State_Change::Enter(CGameObject* pObj, OBJTYPE eType)
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

void CPlayer_State_Change::Update(CGameObject* pObj, float fTimeDelta)
{
	static_cast<CPlayer*>(pObj)->Move_Pos(m_vDir, fTimeDelta);

	m_vDir = { 0.f, 0.f, 0.f,0.f };
}

void CPlayer_State_Change::Exit(CGameObject* pObj)
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pModel);
}

CObject_State* CPlayer_State_Change::Check_Transition(CGameObject* pObj)
{
	// 공격, 대쉬 이런거 다 되도록 추가하기

	if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player")))->IsAnimationFinished())
	{
		m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Player"))->Reset();
		return new CPlayer_State_Idle;
	}

    return nullptr;
}
