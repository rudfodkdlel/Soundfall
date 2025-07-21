#include "Player_State_Ult.h"
#include "Player_State_Idle.h"
#include "Navigation.h"
#include "Projectile_Player.h"

void CPlayer_State_Ult::Enter(CGameObject* pObj)
{
	__super::Enter(pObj);
	m_pPlayer->Get_Range_Weapon()->Set_Active(false);
	m_pPlayer->Get_Melee_Weapon()->Set_Active(true);


	m_eWeaponType = m_pPlayer->Get_Melee_Weapon()->Get_WeaponType();

	if (WEAPON::SICKLE == m_eWeaponType)
	{
		m_pModel->Set_Animation(12, false);

		CProjectile_Player::PROJECTILE_DESC eDesc = {};

		eDesc.vColor = { 1.f,1.f,1.f,1.f };
		XMStoreFloat4(&eDesc.vPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

		m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Ult_Sickle"), m_pGameInstance->Get_Current_Level(),
			TEXT("Layer_Ult_Sickle"), &eDesc);

		m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Ult_Sickle"), m_pGameInstance->Get_Current_Level(),
			TEXT("Layer_Ult_Sickle"), &eDesc);
	}
	else if (WEAPON::AXE == m_eWeaponType)
	{
		m_pModel->Set_Animation(22, true);


	}
}

void CPlayer_State_Ult::Update(CGameObject* pObj, float fTimeDelta)
{

	if (ATTACK::ATTACK_OUT == m_eAttackState)
	{


		if (WEAPON::AXE == m_eWeaponType)
		{
			if (!m_bMakeEffect && m_pModel->Get_Current_Anim_Ratio() > 0.7f)
			{

				CGameObject::GAMEOBJECT_DESC hitDesc = {};
				XMStoreFloat4(&hitDesc.vPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

				(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Crack_Effect"),
					m_pGameInstance->Get_Current_Level(), TEXT("Layer_Effect"), &hitDesc));
				m_bMakeEffect = true;

				m_pGameInstance->StopSound(SOUND_PLAYER_EFFECT);
				m_pGameInstance->PlaySound(TEXT("Artifact_Axe_Base_Prim_Ultimate_Impact.wav"), SOUND_PLAYER_EFFECT, 0.6f);
			}

		}
	}


	if (WEAPON::SICKLE == m_eWeaponType)
	{
		if (ATTACK::ATTACK_IN == m_eAttackState)
		{
			if (m_IsFinish)
			{
				m_pModel->Set_Animation(13, true);
				
				m_eAttackState = ATTACK::ATTACK_LOOP;

			
				

			}
		}
		else if (ATTACK::ATTACK_LOOP == m_eAttackState)
		{
			m_fDuration -= fTimeDelta;

			if (m_fDuration < 0.f)
			{
				m_pModel->Set_Animation(14, false);
				m_eAttackState = ATTACK::ATTACK_OUT;

			}
		}

		auto lastObject = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Ult_Sickle"));

		_vector planePoint = { 0.f,0.f,0.f,1.f };
		_vector planeNoraml = { 0.f,1.f,0.f,0.f };
		m_vMousePos = m_pGameInstance->Get_Mouse_Projection(planePoint, planeNoraml);

		lastObject->Get_Transform()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vMousePos));
	}
	else if (WEAPON::AXE == m_eWeaponType)
	{
		if (m_pGameInstance->Get_DIMouseState(DIM::LBUTTON) & 0x80)
		{
			m_pModel->Set_Animation(23, false);
			m_eAttackState = ATTACK::ATTACK_OUT;

			_vector planePoint = { 0.f,0.f,0.f,1.f };
			_vector planeNoraml = { 0.f,1.f,0.f,0.f };

			m_vMousePos = m_pGameInstance->Get_Mouse_Projection(planePoint, planeNoraml);
			_vector vDir = XMLoadFloat4(&m_vMousePos) -  m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) ;

			XMStoreFloat4(&m_vDir , XMVector3Normalize(vDir));

			m_pGameInstance->StopSound(SOUND_PLAYER_EFFECT);
			m_pGameInstance->PlaySound(TEXT("Artifact_Axe_Brass_Ultimate_Start.wav"), SOUND_PLAYER_EFFECT, 0.6f);
		}

		if (ATTACK::ATTACK_OUT == m_eAttackState)
		{
			_vector vPos = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);

			if (XMVectorGetX( XMVector4Length(XMLoadFloat4(&m_vMousePos) - vPos)) > 5.f)
			{
				vPos += XMLoadFloat4(&m_vDir) * fTimeDelta * 20;

				auto pNavi = static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")));

				if (pNavi == nullptr)
				{
					m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, vPos);
				}
				else
				{

					if (pNavi->isMove(vPos))
						m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, vPos);
				}

			}
			
			
		}
	}

	m_IsFinish = m_pModel->Play_Animation(fTimeDelta);
}

void CPlayer_State_Ult::Exit(CGameObject* pObj)
{
	__super::Exit(pObj);

}

CObject_State* CPlayer_State_Ult::Check_Transition(CGameObject* pObj)
{
	if (m_IsFinish && ATTACK::ATTACK_OUT == m_eAttackState)
	{
		m_pPlayer->Get_Range_Weapon()->Set_Active(true);
		m_pPlayer->Get_Melee_Weapon()->Set_Active(false);

		
	

		auto ultList = m_pGameInstance->GetLayerList(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Ult_Sickle"));

		if (nullptr != ultList)
		{
			for (auto& pObj : *ultList)
			{
				if (nullptr != pObj)
					Safe_Release(pObj);
			}
		}
		return new CPlayer_State_Idle;
	}

	return nullptr;
}
