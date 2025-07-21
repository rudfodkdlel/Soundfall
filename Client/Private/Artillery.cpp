#include "Artillery.h"
#include "GameInstance.h"
#include "Monster_HP.h"
#include "Body_Artillery.h"
#include "Projectile_Artillery.h"
#include "Navigation.h"


CArtillery::CArtillery(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster_Base{pDevice, pContext}
{
}

CArtillery::CArtillery(const CArtillery& Prototype)
    :CMonster_Base{Prototype}
{
}

HRESULT CArtillery::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CArtillery::Initialize(void* pArg)
{
	CONTAINEROBJECT_DESC	Desc{};



	Desc.fRotationPerSec = 90.f;
	Desc.fSpeedPerSec = 5.0f;
	Desc.iNumPartObjects = ENUM_CLASS(PART_DEFAULT::END);
	lstrcpy(Desc.szName, TEXT("Artillery"));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;



	_vector vPos = XMLoadFloat4(&static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg)->vPos);
	m_pTransformCom->Set_State(STATE::POSITION, vPos);


	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;



	m_eMainState = STATE_MAIN::SPWAN;
	Get_BodyModel()->Set_Animation(12, false); 



	return S_OK;
}

void CArtillery::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
	{
		m_pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));
	}

	if (m_bDead)
	{
		m_pGameInstance->StopSound(SOUND_MONSTER_DEATH);
		m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Artillery_Voice_Death_3.wav"), SOUND_MONSTER_DEATH, 1.f);
		Set_Dead();
	}
		


	__super::Priority_Update(fTimeDelta);
}

void CArtillery::Update(_float fTimeDelta)
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
				// IN 애니메이션 끝나면 LOOP 시작
				if (static_cast<CBody_Artillery*>(m_PartObjects[0])->Get_AnimFinished())
				{
					m_eAttackState = ATTACK::ATTACK_LOOP;
					m_fPatternTime = 2.f;
					m_fDuration = 1.f;
					Get_BodyModel()->Set_Animation(1, true); // LOOP

				}
				break;

			case ATTACK::ATTACK_LOOP:
				m_fDuration -= fTimeDelta;
				m_fPatternTime -= fTimeDelta;

				if (m_fDuration <= 0.f)
				{
					m_pGameInstance->StopSound(SOUND_MONSTER_EFFECT);
					m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Artillery_Shot_3.wav"), SOUND_MONSTER_EFFECT, 1.f);

					CProjectile_Artillery::PROJECTILE_DESC eDesc = {};
					eDesc.fSpeedPerSec = 1.f;
					eDesc.vColor = { 1.f,0.f,0.f,0.5f };
					XMStoreFloat4(&eDesc.vPos, m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
					m_pGameInstance->Add_GameObject(
						ENUM_CLASS(LEVEL::STATIC),
						TEXT("Prototype_GameObject_Projectile_Artillery"),
						m_pGameInstance->Get_Current_Level(),
						TEXT("Layer_Projectile_Monster"),
						&eDesc);

					m_fDuration = 1.f;
				}

				if (m_fPatternTime <= 0.f)
				{
					m_eAttackState = ATTACK::ATTACK_OUT;
					Get_BodyModel()->Set_Animation(2, false); // OUT
				}
				break;

			case ATTACK::ATTACK_OUT:
				if (static_cast<CBody_Artillery*>(m_PartObjects[0])->Get_AnimFinished())
				{
					m_eMainState = STATE_MAIN::IDLE;
					Get_BodyModel()->Set_Animation(7, false); // IDLE
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
	}



	__super::Update(fTimeDelta);
}

void CArtillery::Late_Update(_float fTimeDelta)
{
	if (nullptr != m_pTarget)
		Select_State();

	__super::Late_Update(fTimeDelta);
}

HRESULT CArtillery::Render()
{
    return E_NOTIMPL;
}

void CArtillery::Select_State()
{
	if (m_eMainState == STATE_MAIN::ATTACK || !static_cast<CBody_Artillery*>(m_PartObjects[0])->Get_AnimFinished())
	{
		return;
	}

	// 현재 위치
	_vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	if (m_eMainState == STATE_MAIN::SPWAN)
	{
		Get_BodyModel()->Set_Animation(2, false);

		m_eMainState = STATE_MAIN::IDLE;
		return;
	}

	// 거리 제곱 계산
	_vector vLengthSq = XMVector3LengthSq(vTargetPos - vMyPos);

	if (XMVectorGetX(vLengthSq) < 100.0f)
	{
		Get_BodyModel()->Set_Animation(8, false); // MOVE
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

HRESULT CArtillery::Ready_PartObjects()
{

	NORMAL_BODY_DESC BodyDesc{};

	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	BodyDesc.pCombatCom = m_pCombatCom;

	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::BODY), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Artillery"), &BodyDesc)))
		return E_FAIL;


	CMonster_HP::HPBAR_DESC hpDesc{};

	hpDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	hpDesc.pCombatCom = m_pCombatCom;
	/* For.UI */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::UI), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_HP"), &hpDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CArtillery::Ready_Components()
{
	CCombatStat::COMBAT_DESC eDesc = {};
	eDesc.iCurrentHp = 100;
	eDesc.iMaxHp = 100;
	eDesc.iDamage = 0;
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

CArtillery* CArtillery::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArtillery* pInstance = new CArtillery(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CArtillery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CArtillery::Clone(void* pArg)
{
	CArtillery* pInstance = new CArtillery(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CArtillery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CArtillery::Free()
{
	__super::Free();

	Safe_Release(m_pCombatCom);
	Safe_Release(m_pNavigationCom);
}


