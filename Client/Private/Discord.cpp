#include "Discord.h"
#include "GameInstance.h"
#include "Monster_State_Spawn.h"
#include "Monster_Base.h"
#include "PartObject.h"
#include "Body_Discord.h"


CDiscord::CDiscord(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster_Base{ pDevice, pContext }
{
}

CDiscord::CDiscord(const CDiscord& Prototype)
    : CMonster_Base{ Prototype }
{
}

_bool CDiscord::Check_Groggy()
{
	if (m_bUseSummon)
	{
		auto& objList = *m_pGameInstance->GetLayerList(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Boss_Spawn"));
		objList.remove_if([](CGameObject* p)
			{
				return p == nullptr;
			});
		
		if (objList.empty())
			return true;
	}
	

	return false;
}

_bool CDiscord::Check_Wall()
{

	auto& objList = *m_pGameInstance->GetLayerList(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Boss_Wall"));
	objList.remove_if([](CGameObject* p)
		{
			return p == nullptr;
		});

	if (objList.empty())
		return true;

	return false;
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

	

	
	m_pState = new CMonster_State_Spawn;
	m_pState->Enter(this);

	_vector vPos = { 0.f, 0.f, 30.f, 1.f };
	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	_vector vAxis = { 0.f, 1.f, 0.f, 0.f };
	m_pTransformCom->Rotation(vAxis, XMConvertToRadians(180.f));


	return S_OK;
}

void CDiscord::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
	{
		m_pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));
	}
		


	__super::Priority_Update(fTimeDelta);

}

void CDiscord::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (nullptr != m_pState)
		m_pState->Update(this, fTimeDelta);

	if (m_pGameInstance->Key_Up(DIK_H))
	{
		m_pCombatCom->Hit(200);
	}

	
	m_pCombatCom->Update(fTimeDelta);
}

void CDiscord::Late_Update(_float fTimeDelta)
{
	Select_State();

	__super::Late_Update(fTimeDelta);
}

HRESULT CDiscord::Render()
{
	return S_OK;
}

void CDiscord::Select_State()
{
	if (nullptr == m_pState)
		return;

	CObject_State* pState = m_pState->Check_Transition(this);

	if (nullptr != pState)
	{
		m_pState->Exit(this);
		Safe_Delete(m_pState);
		m_pState = pState;
		m_pState->Enter(this);
	}

}

BOSS_PATTERN CDiscord::Get_Next_Skill()
{
	if (m_iIndex >= m_normalSequence.size())
		m_iIndex = 0;

	int iPattern = m_normalSequence[m_iIndex];
	++m_iIndex;

	return BOSS_PATTERN(iPattern);
}

DIR_STATE CDiscord::Get_Hit_Dir()
{
	return DIR_STATE();
}

DIR_STATE CDiscord::Get_Dir_Melee()
{
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vDir = m_pTransformCom->Get_State(STATE::LOOK);
	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

	_vector vToTarget = XMVector3Normalize(vTargetPos - vPos);
	vDir = XMVector3Normalize(vDir);

	_float fDot = XMVectorGetX(XMVector3Dot(vDir, vToTarget));

	_float fTargetX = XMVectorGetX(vTargetPos);
	_float fMyX = XMVectorGetX(vPos);

	if (fTargetX > fMyX)
	{
		if (fDot >= 0.7f)
		{
			return DIR_STATE::FL;
		}
		else
			return DIR_STATE::L;
	}
	else
	{
		if (fDot >= 0.7f)
		{
			return DIR_STATE::FR;
		}
		else
			return DIR_STATE::R;
	}
		



	return DIR_STATE::NONE;
}

HRESULT CDiscord::Ready_PartObjects()
{

	CBody_Discord::BODY_DESC BodyDesc{};
	

	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	BodyDesc.pCombatCom = m_pCombatCom;

	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::BODY), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Discord"), &BodyDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CDiscord::Ready_Components()
{
	CCombatStat::COMBAT_DESC eDesc = {};
	eDesc.iCurrentHp = 1000;
	eDesc.iMaxHp = 1000;
	eDesc.iDamage = 10;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eDesc)))
		return E_FAIL;

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

	Safe_Release(m_pCombatCom);
}
