#pragma once
#include "Player.h"

#include "GameInstance.h"
#include "Model.h"
#include "Weapon_Base.h"
#include "Body_Player.h"
#include "Observer_Animation.h"
#include "Player_State_Spawn.h"
#include "Navigation.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CContainerObject{ Prototype }
{
}

DIR_STATE CPlayer::Get_Hit_Dir()
{
	return static_cast<CBody_Player*>(m_PartObjects[0])->Get_Hit_Dir();
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CONTAINEROBJECT_DESC			Desc{};

	Desc.fRotationPerSec = 90.f;
	Desc.fSpeedPerSec = 10.f;
	Desc.iNumPartObjects = PART_END;
	lstrcpy(Desc.szName, TEXT("Player"));

	m_fSpeed = 10.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	/* 1. 서로 다른 애니메이션을 셋팅했음에도 같은 동작이 재생된다. : 뼈가 공유되기때문에. */
	/* 2. 같은 애니메이션을 셋했다면 재생속도가 빨라진다. : */

	_vector vPos = { 5.f,0.f,5.f,1.f };

	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	m_pState = new CPlayer_State_Spawn;
	m_pState->Enter(this);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	if (m_pCombatCom->Get_Current_HP() <= 0)
		m_pCombatCom->Heal(1.f);

	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{

	Input_Key(fTimeDelta);

	if (nullptr != m_pState)
		m_pState->Update(this, fTimeDelta);

	__super::Update(fTimeDelta);
	
	
	Look_Mouse();

	m_pCombatCom->Update(fTimeDelta); 
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	// 충돌 체크까지 완료하고 상태를 결정 후, 맞는 애니메이션을 선택함
	Select_State();


	if(nullptr != m_PartObjects[0])
		m_PartObjects[0]->Late_Update(fTimeDelta);
	if (nullptr != m_PartObjects[1])
		m_PartObjects[1]->Late_Update(fTimeDelta);
	//대시중에는 꺼야됨
	if (nullptr != m_pRangedWeapon)
		m_pRangedWeapon->Late_Update(fTimeDelta);
	if (nullptr != m_pMeleeWeapon)
		m_pMeleeWeapon->Late_Update(fTimeDelta);

	
	auto pushvectors = static_cast<CBody_Player*>(m_PartObjects[0])->Get_PushVectors();
	if (!pushvectors.empty())
	{
		_vector vSumDir = {};

		for (auto& pushvector : pushvectors)
		{
			vSumDir += XMLoadFloat4(&pushvector);
		}

		_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

		vPos += vSumDir;
		if (m_pNavigationCom->isMove(vPos))
			m_pTransformCom->Set_State(STATE::POSITION, vPos);

		static_cast<CBody_Player*>(m_PartObjects[0])->Clear_PushVectors();
	}
	


	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
	if (m_fRenderTime > 0.f)
	{
		if (m_IsGoodTiming)
		{
			m_pGameInstance->Draw_Font(TEXT("Default"), TEXT("GOOD Timing"), _float2(10.f, 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}
		else
		{
			m_pGameInstance->Draw_Font(TEXT("Default"), TEXT("BAD Timing"), _float2(10.f, 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}
	}
	
	return S_OK;
}

void CPlayer::Input_Key(_float fTimeDelta)
{
	if (!m_pGameInstance->Is_NoKeyPressed())
	{
		m_fRenderTime = 0.5f;

		if ((fabs(m_pGameInstance->Get_Timing()) < 0.15f))
		{
			m_IsGoodTiming = true;
		}
		else
			m_IsGoodTiming = false;

	}

	
	if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{
		return;
	}
	
	if (m_pGameInstance->Is_NoKeyPressed())
	{
		m_fRenderTime -= fTimeDelta;
		return;
	}

	Select_Weapon();
	




}

void CPlayer::Select_State()
{
	if (nullptr == m_pState)
		return;

	CObject_State* pNextState = m_pState->Check_Transition(this);

	if (nullptr != pNextState)
	{
		m_pState->Exit(this);
		Safe_Delete(m_pState);

		m_pState = pNextState;
		m_pState->Enter(this);
	}
	

}

void CPlayer::Move_Pos(_vector& vDir, _float fTimeDelta, _float fSpeedRatio)
{
	if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		vDir += {-1.f, 0.f, 0.f, 0.f};


	}
	if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		vDir += {1.f, 0.f, 0.f, 0.f};

	}
	if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		vDir += {0.f, 0.f, 1.f, 0.f};

	}
	if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		vDir += {0.f, 0.f, -1.f, 0.f};

	}

	if (0.f < XMVectorGetX(XMVector3Length(vDir)))
		vDir = XMVector3Normalize(vDir);

	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	vPos += vDir * fTimeDelta * m_fSpeed * fSpeedRatio;

	if(m_pNavigationCom->isMove(vPos))
		m_pTransformCom->Set_State(STATE::POSITION, vPos);

}

void CPlayer::Select_Weapon()
{
	if (m_pGameInstance->Get_DIKeyState(DIK_1) & 0x80)
	{
		m_pRangedWeapon = static_cast<CWeapon_Base*>(m_PartObjects[PART_WEAPON_0]);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_2) & 0x80)
	{
		m_pRangedWeapon = static_cast<CWeapon_Base*>(m_PartObjects[PART_WEAPON_1]);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_3) & 0x80)
	{
		m_pMeleeWeapon = static_cast<CWeapon_Base*>(m_PartObjects[PART_WEAPON_MELEE_0]);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_4) & 0x80)
	{
		m_pMeleeWeapon = static_cast<CWeapon_Base*>(m_PartObjects[PART_WEAPON_MELEE_1]);
	}
}

void CPlayer::Toggle_Collider(_bool IsActive)
{
	static_cast<CCollider*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Collider")))->Set_Active(IsActive);
}

_bool CPlayer::Is_Body_Collision()
{
	return static_cast<CCollider*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Collider")))->Get_IsColl();
}

void CPlayer::Look_Mouse()
{
	_float4 MousePos = {};
	_vector vPlanePoint = { 0.f, 0.f, 0.f, 1.f };
	_vector vDir = { 0.f, 1.f, 0.f, 0.f };
	MousePos = m_pGameInstance->Get_Mouse_Projection(vPlanePoint, vDir);

	if (MousePos.w != 0.f)
		m_pTransformCom->LookAt(XMLoadFloat4(&MousePos));

}

_bool CPlayer::Check_Move()
{
	if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80 || m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80 || 
		m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80 || m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{


		return true;
	}
	return false;
}

DIR_STATE CPlayer::Calc_Dir(_vector vDir)
{
	
	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
	vLook = XMVector3Normalize(vLook);

	if (XMVector3Equal(vDir, XMVectorZero()))
		return NONE;

	_float dot = XMVectorGetX(XMVector3Dot(vDir, vLook));
	_float x = XMVectorGetX(vDir);

	// 기준 각도 값
	const _float COS_45 = 0.707f;
	const _float COS_135 = -0.707f;

	// 앞
	if (dot >= COS_45)
	{
		if (x > 0)      return FR;
		else if (x < 0) return FL;
		else            return F;
	}
	// 좌우
	else if (dot > COS_135)
	{
		if (x > 0)      return R;
		else if (x < 0) return L;
	}
	// 뒤
	else
	{
		if (x > 0)      return BR;
		else if (x < 0) return BL;
		else            return B;
	}


	return DIR_STATE::NONE;
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* For.Body */
	CBody_Player::BODY_PLAYER_DESC BodyDesc{};

	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	BodyDesc.pCombatcom = m_pCombatCom;

	if (FAILED(__super::Add_PartObject(PART_BODY, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
		return E_FAIL;

	/* For.Weapon */
	CWeapon_Base::WEAPON_DESC	WeaponDesc{};

	WeaponDesc.pSocketMatrix = dynamic_cast<CBody_Player*>(m_PartObjects[PART_BODY])->Get_SocketMatrix("rt_weapon_SOCKET_jnt");

	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();

	if (FAILED(__super::Add_PartObject(PART_WEAPON_0, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Rifle"), &WeaponDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_PartObject(PART_WEAPON_1, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Keyboard"), &WeaponDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_PartObject(PART_WEAPON_MELEE_0, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Sickle"), &WeaponDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_PartObject(PART_WEAPON_MELEE_1, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Axe"), &WeaponDesc)))
		return E_FAIL;

	m_pRangedWeapon = static_cast<CWeapon_Base*>(m_PartObjects[PART_WEAPON_0]);

	m_pMeleeWeapon = static_cast<CWeapon_Base*>(m_PartObjects[PART_WEAPON_MELEE_0]);
	m_pMeleeWeapon->Set_Active(false);

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	CCombatStat::COMBAT_DESC eDesc = {};
	eDesc.iCurrentHp = 100;
	eDesc.iMaxHp = 100;
	eDesc.iDamage = 10;
	eDesc.fInvincibleDuration = 1.f;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC		NaviDesc{};
	NaviDesc.iIndex = 0;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	return S_OK;

}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	if (nullptr != m_pState)
	{
		CObject_State* pOldState = m_pState;
		m_pState = nullptr;

		pOldState->Exit(this);
		Safe_Delete(pOldState);
	}

	__super::Free();



	Safe_Release(m_pCombatCom);
	Safe_Release(m_pNavigationCom);

	

	
	
}
