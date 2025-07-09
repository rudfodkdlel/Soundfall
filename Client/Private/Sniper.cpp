#include "Sniper.h"
#include "GameInstance.h"
#include "Body_Sniper.h"
#include "Weapon_Sniper.h"
#include "Monster_HP.h"
#include "Navigation.h"
#include "Attack_Area_Monster.h"
#include "Projectile_Monster.h"
#include "Attack_Area_Monster.h"

CSniper::CSniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster_Base{pDevice, pContext}
{
}

CSniper::CSniper(const CSniper& Prototype)
    :CMonster_Base{Prototype}
{
}

HRESULT CSniper::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSniper::Initialize(void* pArg)
{
	CONTAINEROBJECT_DESC	Desc{};



	Desc.fRotationPerSec = 90.f;
	Desc.fSpeedPerSec = 5.0f;
	Desc.iNumPartObjects = ENUM_CLASS(PART_DEFAULT::END);
	lstrcpy(Desc.szName, TEXT("Defender"));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	_vector vPos = XMLoadFloat4(&static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg)->vPos);
	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;







	return S_OK;
}

void CSniper::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
	{
		m_pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));
	}

	if (m_bDead)
		Set_Dead();


	__super::Priority_Update(fTimeDelta);
}

void CSniper::Update(_float fTimeDelta)
{

	m_bDead = m_pCombatCom->Update(fTimeDelta);

	// 상태 에 따른 행동
	if (nullptr != m_pTarget)
	{

		if (m_eMainState == STATE_MAIN::ATTACK)
		{
			switch (m_eAttackState)
			{
			case ATTACK::ATTACK_IN:
				static_cast<CAttack_Area_Monster*>(m_PartObjects[ENUM_CLASS(PART_DEFAULT::EFFECT)])->Set_IsActive(true);
				m_pTransformCom->LookAt(m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
				// IN 애니메이션 끝나면 LOOP 시작
				if (static_cast<CBody_Sniper*>(m_PartObjects[0])->Get_AnimFinished())
				{
					m_eAttackState = ATTACK::ATTACK_OUT;
					m_fPatternTime = 2.f;
					
					Get_BodyModel()->Set_Animation(0, false); // LOOP
				}
				break;

			case ATTACK::ATTACK_OUT:
				if (static_cast<CBody_Sniper*>(m_PartObjects[0])->Get_AnimRatio() > 0.7f && !m_isShoot)
				{
					CProjectile_Monster::PROJECTILE_DESC eDesc = {};
					eDesc.fSpeedPerSec = 4.f;

					eDesc.vColor = { 1.f,0.f,0.f,1.f };

					_vector vDir = m_pTransformCom->Get_State(STATE::LOOK);

					vDir = XMVector3Normalize(vDir);

					_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
					vPos.m128_f32[1] += 1.f;

					XMStoreFloat4(&eDesc.vPos, vPos);
					XMStoreFloat4(&eDesc.vDir, vDir);
					// 투사체 생성해서 날아가게 해보자
					m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Monster"), m_pGameInstance->Get_Current_Level(),
						TEXT("Layer_Projectile_Monster"), &eDesc);
					m_isShoot = true;
				}

				if (static_cast<CBody_Sniper*>(m_PartObjects[0])->Get_AnimFinished())
				{
					m_eMainState = STATE_MAIN::IDLE;
					Get_BodyModel()->Set_Animation(4, false); // IDLE

					

					static_cast<CAttack_Area_Monster*>(m_PartObjects[ENUM_CLASS(PART_DEFAULT::EFFECT)])->Set_IsActive(false);
					m_isShoot = false;
				}
				break;
			}
		}
		else if (m_eMainState == STATE_MAIN::MOVE)
		{

			_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
			m_pTransformCom->LookAt(vTargetPos);
			if (m_eDirState == eDirState::B)
			{
				m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);
			}
			else if (m_eDirState == eDirState::F)
			{
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}
		}
		else if(m_eMainState == STATE_MAIN::IDLE)
			m_pTransformCom->LookAt(m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
	}



	__super::Update(fTimeDelta);
}

void CSniper::Late_Update(_float fTimeDelta)
{
	if (nullptr != m_pTarget)
		Select_State();


	__super::Late_Update(fTimeDelta);
}

HRESULT CSniper::Render()
{
    return E_NOTIMPL;
}

void CSniper::Select_State()
{
	if(m_eMainState == STATE_MAIN::ATTACK || !static_cast<CBody_Sniper*>(m_PartObjects[0])->Get_AnimFinished())
	{
		return;
	}

	// 현재 위치
	_vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	if (m_eMainState == STATE_MAIN::SPWAN)
	{
		Get_BodyModel()->Set_Animation(4, false);

		m_eMainState = STATE_MAIN::IDLE;
		return;
	}

	// 거리 제곱 계산
	_vector vLengthSq = XMVector3LengthSq(vTargetPos - vMyPos);

	if (XMVectorGetX(vLengthSq) < 100.0f)
	{
		Get_BodyModel()->Set_Animation(12, false); // MOVE
		m_eMainState = STATE_MAIN::MOVE;
		m_eDirState = eDirState::B;
	}
	else
	{
		Get_BodyModel()->Set_Animation(1, false); // ATTACK
		m_eMainState = STATE_MAIN::ATTACK;
		m_eAttackState = ATTACK::ATTACK_IN;
	}
}

HRESULT CSniper::Ready_PartObjects()
{
	NORMAL_BODY_DESC BodyDesc{};

	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	BodyDesc.pCombatCom = m_pCombatCom;

	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::BODY), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Sniper"), &BodyDesc)))
		return E_FAIL;



	/* For.Weapon */
	CWeapon_Sniper::WEAPON_DESC	WeaponDesc{};

	WeaponDesc.pSocketMatrix = dynamic_cast<CBody_Sniper*>(m_PartObjects[0])->Get_SocketMatrix("rt_weapon_SOCKET_jnt");

	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	WeaponDesc.eMainState = &m_eMainState;

	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::WEAPON), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Sniper_Weapon"), &WeaponDesc)))
		return E_FAIL;

	CMonster_HP::HPBAR_DESC hpDesc{};

	hpDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	hpDesc.pCombatCom = m_pCombatCom;
	/* For.UI */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::UI), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_HP"), &hpDesc)))
		return E_FAIL;



	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::EFFECT), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Attack_Area_Monster"), &hpDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSniper::Ready_Components()
{
	CCombatStat::COMBAT_DESC eDesc = {};
	eDesc.iCurrentHp = 100;
	eDesc.iMaxHp = 100;
	eDesc.iDamage = 10;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC		NaviDesc{};
	NaviDesc.iIndex = -1;
	XMStoreFloat4(&NaviDesc.vInitPos, m_pTransformCom->Get_State(STATE::POSITION));

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::FOREST), TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	return S_OK;

}

CSniper* CSniper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSniper* pInstance = new CSniper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSniper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSniper::Clone(void* pArg)
{
	CSniper* pInstance = new CSniper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSniper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSniper::Free()
{
	__super::Free();

	Safe_Release(m_pCombatCom);
	Safe_Release(m_pNavigationCom);
}

