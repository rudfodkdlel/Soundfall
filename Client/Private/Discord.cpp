#include "Discord.h"
#include "GameInstance.h"
#include "Object_State_Spawn.h"
#include "Monster_Base.h"
#include "PartObject.h"
#include "Observer_Animation.h"
#include "Observer_State.h"
#include "Observer_Phase.h"
#include "Model.h"

CDiscord::CDiscord(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster_Base{ pDevice, pContext }
{
}

CDiscord::CDiscord(const CDiscord& Prototype)
    : CMonster_Base{ Prototype }
{
}

HRESULT CDiscord::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDiscord::Initialize(void* pArg)
{
	CONTAINEROBJECT_DESC	Desc{};

	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 5.0f;
	Desc.iNumPartObjects = ENUM_CLASS(PART_DEFAULT::END);
	lstrcpy(Desc.szName, TEXT("Discord"));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	//m_pState = new CObject_State_Spawn;
	//m_pState->Enter(this, OBJTYPE::MOSNTER);

	m_pGameInstance->Add_Observer(TEXT("Observer_Animation_Discord"), new CObserver_Animation);
	m_pGameInstance->Add_Observer(TEXT("Observer_Phase_Discord"), new CObserver_Phase);
	m_pGameInstance->Add_Observer(TEXT("Observer_State_Discord"), new CObserver_State);

	return S_OK;
}

void CDiscord::Priority_Update(_float fTimeDelta)
{
	
		

	__super::Priority_Update(fTimeDelta);

}

void CDiscord::Update(_float fTimeDelta)
{
	// 일단 임시로 key 받아서 페이즈 바뀌게 하기

	if (m_pGameInstance->Get_DIKeyState(DIK_1) & 0x80)
		m_iPhase = 1;
	if (m_pGameInstance->Get_DIKeyState(DIK_2) & 0x80)
		m_iPhase = 2;
	if (m_pGameInstance->Get_DIKeyState(DIK_3) & 0x80)
		m_iPhase = 3;


	Select_State();

	__super::Update(fTimeDelta);
}

void CDiscord::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CDiscord::Render()
{
	return S_OK;
}

void CDiscord::Select_State()
{
	CModel* pModel = static_cast<CModel*>(m_PartObjects[ENUM_CLASS(PART_DEFAULT::BODY)]->Get_Component(TEXT("Com_Model")));
	if (nullptr == pModel)
		return;

	switch (m_eMainState)
	{
	case Client::STATE_MAIN::IDLE:
		m_iAttackPattern = int(m_pGameInstance->Compute_Random(0.f, 4.9f));

		switch (m_iAttackPattern)
		{
		case 0:
			if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
			{
				pModel->Set_Animation(3, true);
				m_eMainState = STATE_MAIN::ATTACK;
				static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->Reset();
			}
			break;
		case 1:
			if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
			{
				pModel->Set_Animation(5, true);
				m_eMainState = STATE_MAIN::ATTACK;
				static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->Reset();
			}
			break;
		case 2:
			if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
			{
				pModel->Set_Animation(8, true);
				m_eMainState = STATE_MAIN::ATTACK;
				static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->Reset();
			}
			break;
		case 3:
			if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
			{
				pModel->Set_Animation(19, true);
				m_eMainState = STATE_MAIN::ATTACK;
				static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->Reset();
			}
			break;
		case 4:
			if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
			{
				pModel->Set_Animation(2, true);
				m_eMainState = STATE_MAIN::ATTACK;
				static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->Reset();
			}
			break;

		default:
			break;
		}

		break;
	case Client::STATE_MAIN::ATTACK:
		if (m_iPhase == 1)
		{
			if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
			{
				pModel->Set_Animation(30, false);
				m_eMainState = STATE_MAIN::IDLE;
				static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->Reset();
			}
		}
		else if (m_iPhase == 2)
		{
			if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
			{
				pModel->Set_Animation(31, false);
				m_eMainState = STATE_MAIN::IDLE;
				static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->Reset();
			}
		}
		else if (m_iPhase == 3)
		{
			if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
			{
				pModel->Set_Animation(32, false);
				m_eMainState = STATE_MAIN::IDLE;
				static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->Reset();
			}
		}
		break;
	case Client::STATE_MAIN::HIT:
		break;
	case Client::STATE_MAIN::STUN:
		break;
	case Client::STATE_MAIN::SPWAN:
		if (static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->IsAnimationFinished())
		{
			pModel->Set_Animation(30, false);
			m_eMainState = STATE_MAIN::IDLE;
			static_cast<CObserver_Animation*>(m_pGameInstance->Find_Observer(TEXT("Observer_Animation_Discord")))->Reset();
		}
		break;
	case Client::STATE_MAIN::DEAD:
		break;
	default:
		break;
	}

}

HRESULT CDiscord::Ready_PartObjects()
{
	CPartObject::PARTOBJECT_DESC	BodyDesc{};

	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();

	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::BODY), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Discord"), &BodyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDiscord::Ready_Components()
{
    return S_OK;
}

CDiscord* CDiscord::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDiscord* pInstance = new CDiscord(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDiscord");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDiscord::Clone(void* pArg)
{
	CDiscord* pInstance = new CDiscord(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDiscord");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDiscord::Free()
{
	__super::Free();

	if (nullptr != m_pState)
	{
		m_pState->Exit(this);
		Safe_Delete(m_pState);
	}
}
