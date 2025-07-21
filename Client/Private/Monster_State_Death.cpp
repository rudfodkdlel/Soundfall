#include "Monster_State_Death.h"

void CMonster_State_Death::Enter(CGameObject* pObj)
{
    __super::Enter(pObj);
    m_pModel->Set_Animation(20, false);

	m_pGameInstance->Find_Observer(TEXT("Observer_Trigger"))->OnNotify(TEXT("ZOOM_OUT"));

	auto objList =  m_pGameInstance->GetLayerList(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Boss_UI"));

	for (auto& pObj : *objList)
	{
		if (nullptr != pObj || !(pObj->Get_Dead()))
		{
			pObj->Set_Dead();
		}
	}

	m_pGameInstance->StopSound(SOUND_BOSS);
	m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Discord_Death_VO.wav"), SOUND_BOSS, 1.f);
}

void CMonster_State_Death::Update(CGameObject* pObj, float fTimeDelta)
{
	if (m_eAnimState == ANIMSTATE::ANIM_IN)
	{
		if (m_IsFinish)
		{
			m_eAnimState = ANIMSTATE::ANIM_LOOP;
			m_pModel->Set_Animation(21, true);
		}
	
	}
	else if (m_eAnimState == ANIMSTATE::ANIM_LOOP)
	{
		m_fDuration -= fTimeDelta;

		
	}

	if (m_eAnimState == ANIMSTATE::ANIM_IN || m_eAnimState == ANIMSTATE::ANIM_LOOP)
	{
		if (m_fDuration < 0)
		{
			m_eAnimState = ANIMSTATE::ANIM_OUT;
			m_pModel->Set_Animation(22, false);
		}

		
	}
	if(m_eAnimState != ANIMSTATE::ANIM_OUT)
		m_IsFinish = m_pModel->Play_Animation(fTimeDelta * 0.3f);
	else
		m_IsFinish = m_pModel->Play_Animation(fTimeDelta * 0.05f);
}

void CMonster_State_Death::Exit(CGameObject* pObj)
{
    __super::Exit(pObj);
}

CObject_State* CMonster_State_Death::Check_Transition(CGameObject* pObj)
{
	if (ANIMSTATE::ANIM_OUT == m_eAnimState && m_IsFinish)
	{
		pObj->Set_Dead();
		return nullptr;
	}
	return nullptr;
}
