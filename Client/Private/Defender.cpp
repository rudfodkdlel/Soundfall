#include "Defender.h"
#include "GameInstance.h"
#include "Body_Defender.h"
#include "Weapon_Defender.h"
#include "Monster_HP.h"

CDefender::CDefender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster_Base{pDevice, pContext}
{
}

CDefender::CDefender(const CDefender& Prototype)
    :CMonster_Base{Prototype}
{
}

HRESULT CDefender::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDefender::Initialize(void* pArg)
{
	CONTAINEROBJECT_DESC	Desc{};



	Desc.fRotationPerSec = 90.f;
	Desc.fSpeedPerSec = 5.0f;
	Desc.iNumPartObjects = ENUM_CLASS(PART_DEFAULT::END);
	lstrcpy(Desc.szName, TEXT("Defender"));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	_vector vPos = { 10.f,0.f,0.f,1.f };

	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	

	return S_OK;
}

void CDefender::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
	{
		m_pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));
	}

	if(m_bDead)
		Set_Dead();


	__super::Priority_Update(fTimeDelta);
}

void CDefender::Update(_float fTimeDelta)
{

	m_bDead =m_pCombatCom->Update(fTimeDelta);


	if (nullptr != m_pTarget)
	{
		if (STATE_MAIN::MOVE == m_eMainState)
		{
			m_pTransformCom->LookAt(m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		else if (STATE_MAIN::IDLE == m_eMainState)
		{
			// 내 방향
			_vector vMyLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));

			// 목표 방향
			_vector vToTarget = XMVector3Normalize(
				m_pTarget->Get_Transform()->Get_State(STATE::POSITION) -
				m_pTransformCom->Get_State(STATE::POSITION)
			);

			// 월드 Y축 기준 외적
			_vector vCross = XMVector3Cross(vMyLook, vToTarget);
			float fY = XMVectorGetY(vCross);

			_vector vDir = {};

			if (fY > 0)
			{
				vDir = { 0.f,1.f,0.f,0.f };

				
			}
			else if (fY < 0)
			{
				vDir = { 0.f,-1.f,0.f,0.f };
			}
			m_pTransformCom->Turn(vDir, fTimeDelta * 0.05f);
		}
	}

	
		

	__super::Update(fTimeDelta);
}

void CDefender::Late_Update(_float fTimeDelta)
{
	Select_State();

	auto pushvectors = static_cast<CBody_Defender*>(m_PartObjects[0])->Get_PushVectors();
	if (!pushvectors.empty() && m_eMainState != STATE_MAIN::ATTACK)
	{
		_vector vSumDir = {};

		for (auto& pushvector : pushvectors)
		{
			vSumDir += XMLoadFloat4(&pushvector) * 0.2f;
		}

		_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

		vPos += vSumDir;

		m_pTransformCom->Set_State(STATE::POSITION, vPos);

		static_cast<CBody_Defender*>(m_PartObjects[0])->Clear_PushVectors();
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CDefender::Render()
{
    return S_OK;
}

void CDefender::Select_State()
{
	
	if (m_eMainState != STATE_MAIN::IDLE && m_eMainState != STATE_MAIN::MOVE &&
		!static_cast<CBody_Defender*>(m_PartObjects[0])->Get_AnimFinished())
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

	if (XMVectorGetX(vLengthSq) > 64.0f)
	{
		Get_BodyModel()->Set_Animation(7, false); // MOVE
		m_eMainState = STATE_MAIN::MOVE;
	}
	else
	{
		Get_BodyModel()->Set_Animation(1, false); // ATTACK
		m_eMainState = STATE_MAIN::ATTACK;
	}

}

HRESULT CDefender::Ready_PartObjects()
{

	NORMAL_BODY_DESC BodyDesc{};

	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	BodyDesc.pCombatCom = m_pCombatCom;

	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::BODY), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Defender"), &BodyDesc)))
		return E_FAIL;

	

	/* For.Weapon */
	CWeapon_Defender::WEAPON_DESC	WeaponDesc{};

	WeaponDesc.pSocketMatrix = dynamic_cast<CBody_Defender*>(m_PartObjects[0])->Get_SocketMatrix("rt_weapon_SOCKET_jnt");

	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	WeaponDesc.eMainState = &m_eMainState;

	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::WEAPON), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Defender_Weapon"), &WeaponDesc)))
		return E_FAIL;

	CMonster_HP::HPBAR_DESC hpDesc{};

	hpDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	hpDesc.pCombatCom = m_pCombatCom;
	/* For.UI */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::UI), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_HP"), &hpDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CDefender::Ready_Components()
{
	CCombatStat::COMBAT_DESC eDesc = {};
	eDesc.iCurrentHp = 100;
	eDesc.iMaxHp = 100;
	eDesc.iDamage = 10;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom),&eDesc)))
		return E_FAIL;

    return S_OK;
}

CDefender* CDefender::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDefender* pInstance = new CDefender(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDefender");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDefender::Clone(void* pArg)
{
	CDefender* pInstance = new CDefender(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDefender");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefender::Free()
{
	__super::Free();

	Safe_Release(m_pCombatCom);
}

