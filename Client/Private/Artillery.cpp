#include "Artillery.h"
#include "GameInstance.h"
#include "Monster_HP.h"
#include "Body_Artillery.h"
#include "Projectile_Artillery.h"

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

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	_vector vPos = { 10.f,0.f,10.f,1.f };

	m_pTransformCom->Set_State(STATE::POSITION, vPos);



	return S_OK;
}

void CArtillery::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
	{
		m_pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));
	}

	if (m_bDead)
		Set_Dead();


	__super::Priority_Update(fTimeDelta);
}

void CArtillery::Update(_float fTimeDelta)
{
	m_bDead = m_pCombatCom->Update(fTimeDelta);

	// 상태 에 따른 행동
	if (nullptr != m_pTarget)
	{
		if (m_eAttackState == ATTACK::ATTACK_LOOP && m_eMainState == STATE_MAIN::ATTACK)
		{
			m_fDuration -= fTimeDelta;
			m_fPatternTime -= fTimeDelta;

			if (m_fDuration < 0.f)
			{
				CProjectile_Artillery::PROJECTILE_DESC eDesc = {};
				eDesc.fSpeedPerSec = 1.f;

				_vector vPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

				XMStoreFloat4(&eDesc.vPos, vPos);
				
				// 투사체 생성해서 날아가게 해보자
				m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Artillery"), m_pGameInstance->Get_Current_Level(),
					TEXT("Layer_Projectile_Monster"), &eDesc);

				m_fDuration = 0.2f;
			}

			if (m_fPatternTime <= 0.f)
			{
				m_eAttackState = ATTACK::ATTACK_OUT;
				Get_BodyModel()->Set_Animation(2, false);
			}

			
		}
		else
		{
			if (m_eMainState == STATE_MAIN::ATTACK && m_eAttackState == ATTACK::ATTACK_OUT &&
				!static_cast<CBody_Artillery*>(m_PartObjects[0])->Get_AnimFinished())
				return;


			// 거리 계산
			float fDistanceSq = XMVectorGetX(XMVector3LengthSq(m_pTarget->Get_Transform()->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION)));

			if (fDistanceSq < 64.0f)
			{
				// 가까우면 후진
				Get_BodyModel()->Set_Animation(8, false);
				m_eMainState = STATE_MAIN::MOVE;
				m_eDirState = eDirState::B;
			}
			else if (fDistanceSq > 128.0f)
			{
				// 멀면 전진
				Get_BodyModel()->Set_Animation(9, false);
				m_eMainState = STATE_MAIN::MOVE;
				m_eDirState = eDirState::F;
			}
			else
			{
				// 공격
				Get_BodyModel()->Set_Animation(0, false);
				m_eMainState = STATE_MAIN::ATTACK;
				m_eAttackState = ATTACK::ATTACK_IN;
			}





			// 실제 상태 처리
			if (STATE_MAIN::MOVE == m_eMainState)
			{
				m_pTransformCom->LookAt(m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
				if (m_eDirState == eDirState::F)
					m_pTransformCom->Go_Straight(fTimeDelta);
				else
					m_pTransformCom->Go_Backward(fTimeDelta);
			}
			else if (STATE_MAIN::ATTACK == m_eMainState)
			{
				switch (m_eAttackState)
				{
				case ATTACK::ATTACK_IN:
					if (static_cast<CBody_Artillery*>(m_PartObjects[0])->Get_AnimFinished())
					{
						m_eAttackState = ATTACK::ATTACK_LOOP;
						m_fPatternTime = 2.f;
						Get_BodyModel()->Set_Animation(1, true);
					}
					break;



				case ATTACK::ATTACK_OUT:
					if (static_cast<CBody_Artillery*>(m_PartObjects[0])->Get_AnimFinished())
					{
						m_eMainState = STATE_MAIN::IDLE;
						Get_BodyModel()->Set_Animation(7, false);
					}

					break;
				}
			}
		}
	}



	__super::Update(fTimeDelta);
}

void CArtillery::Late_Update(_float fTimeDelta)
{

	__super::Late_Update(fTimeDelta);
}

HRESULT CArtillery::Render()
{
    return E_NOTIMPL;
}

void CArtillery::Select_State()
{
	if (m_eMainState != STATE_MAIN::IDLE && m_eMainState != STATE_MAIN::MOVE &&
		!static_cast<CBody_Artillery*>(m_PartObjects[0])->Get_AnimFinished())
		return;

	// 현재 위치
	_vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	// 타겟까지의 벡터
	_vector vToTarget = XMVector3Normalize(vTargetPos - vMyPos);

	// 내 현재 look 방향
	_vector vMyLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));

	// 각도 차이 확인 (0이면 정면, 1이면 90도)
	float fDot = XMVectorGetX(XMVector3Dot(vMyLook, vToTarget));
	float fAngle = acosf(fDot); // 라디안

	// 일정 각도 이상 차이 나면 TURN 상태 (예: 5도 이상)
	if (fAngle > XMConvertToRadians(10.0f))
	{
		Get_BodyModel()->Set_Animation(2, false);

		m_eMainState = STATE_MAIN::IDLE;
		return;
	}

	// 거리 제곱 계산
	_vector vLengthSq = XMVector3LengthSq(vTargetPos - vMyPos);

	if (XMVectorGetX(vLengthSq) < 64.0f)
	{
		Get_BodyModel()->Set_Animation(8, false); // MOVE
		m_eMainState = STATE_MAIN::MOVE;
		m_eDirState = eDirState::B;
	}
	else if (XMVectorGetX(vLengthSq) > 128.0f)
	{
		Get_BodyModel()->Set_Animation(9, false); // MOVE
		m_eMainState = STATE_MAIN::MOVE;
		m_eDirState = eDirState::F;
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
}


