#include "Tentacle_Amp.h"
#include "GameInstance.h"
#include "CombatStat.h"
#include "Monster_HP.h"
#include "Projectile_Monster.h"

CTentacle_Amp::CTentacle_Amp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster_Base{pDevice, pContext}
{
}

CTentacle_Amp::CTentacle_Amp(const CTentacle_Amp& Prototype)
    :CMonster_Base{Prototype}
{
}

HRESULT CTentacle_Amp::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTentacle_Amp::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CMonster_Base::CONTAINEROBJECT_DESC* pDesc = static_cast<CONTAINEROBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;


	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vPos));

	m_pModelCom->Set_Animation(3, false);

	m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom, this);

	return S_OK;
}

void CTentacle_Amp::Priority_Update(_float fTimeDelta)
{
	// target 설정
	if (!m_IsTargeted)
	{
		m_pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));
		Safe_AddRef(m_pTarget);

		if (nullptr != m_pTarget)
			m_IsTargeted = true;
		// 못찾으면 return
	}

	if (STATE_MAIN::DEAD == m_eMainState && m_IsFinished)
		Set_Dead();

	__super::Priority_Update(fTimeDelta);
}

void CTentacle_Amp::Update(_float fTimeDelta)
{
	if (m_IsTargeted)
	{
		m_pTransformCom->LookAt(m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
	}

	if (m_eMainState != STATE_MAIN::ATTACK)
		m_IsFinished = m_pModelCom->Play_Animation(fTimeDelta);
	else
		m_IsFinished = m_pModelCom->Play_Animation(fTimeDelta * 0.5f);

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));

	m_bDead = m_pCombatCom->Update(fTimeDelta);

	if (m_eMainState == STATE_MAIN::ATTACK)
	{
		m_fDelay += fTimeDelta;
		m_fAttackTime -= fTimeDelta;

		if (m_fAttackTime < 0.f)
		{
			m_eMainState = STATE_MAIN::IDLE;
			m_pModelCom->Set_Animation(0, false);
			m_fAttackTime = 1.5f;
		}

		if (m_fDelay > 0.75f)
		{

			CProjectile_Monster::PROJECTILE_DESC eDesc = {};
			eDesc.fSpeedPerSec = 1.f;
			
			eDesc.vColor = { 1.f,0.f,0.f,1.f };
			
			_vector vDir = m_pTransformCom->Get_State(STATE::LOOK);
			
			vDir = XMVector3Normalize(vDir);

			XMStoreFloat4(&eDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
			XMStoreFloat4(&eDesc.vDir, vDir);
			// 투사체 생성해서 날아가게 해보자
			m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Monster"), m_pGameInstance->Get_Current_Level(),
				TEXT("Layer_Projectile_Monster"), &eDesc);
			 // 총 생성
			m_fDelay = 0.f;
		}
	}

	__super::Update(fTimeDelta);
}

void CTentacle_Amp::Late_Update(_float fTimeDelta)
{
	Select_State();

	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CTentacle_Amp::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;



	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, 1, 0)))
			return E_FAIL;

		m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG

	m_pColliderCom->Render();


#endif

	return S_OK;
}

HRESULT CTentacle_Amp::On_Collision(CCollider* pCollider)
{
    return S_OK;
}

void CTentacle_Amp::Select_State()
{
	if (m_bDead)
	{
		m_eMainState = STATE_MAIN::DEAD;
		m_pModelCom->Set_Animation(1, false);
		return;
	}

	if (STATE_MAIN::DEAD == m_eMainState)
		return;

	

	if (m_IsFinished)
	{
		switch (m_eMainState)
		{
		case Client::STATE_MAIN::IDLE:
			m_eMainState = STATE_MAIN::ATTACK;
			m_pModelCom->Set_Animation(7, true);

			// 공격 받으면 이제 idle에서만 바꾸기

			break;
		case Client::STATE_MAIN::ATTACK:
			m_eMainState = STATE_MAIN::IDLE;
			m_pModelCom->Set_Animation(0, false);

			break;
		case Client::STATE_MAIN::HIT:
			m_eMainState = STATE_MAIN::IDLE;
			m_pModelCom->Set_Animation(5, false);
			break;
		case Client::STATE_MAIN::SPWAN:
			m_eMainState = STATE_MAIN::IDLE;
			m_pModelCom->Set_Animation(0, false);
			break;
		default:
			break;
		}
	}
}

HRESULT CTentacle_Amp::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Tentacle_Amp"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;



	CBounding_AABB::AABB_DESC eDesc{};
	eDesc.vExtents = { 2.f, 10.f,2.f };
	eDesc.vCenter = { 0.f,2.f,0.f };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com"), reinterpret_cast<CComponent**>(&m_pColliderCom), &eDesc)))
		return E_FAIL;

	CCombatStat::COMBAT_DESC eCombatDesc = {};
	eCombatDesc.iCurrentHp = 80;
	eCombatDesc.iMaxHp = 80;
	eCombatDesc.iDamage = 0;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eCombatDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTentacle_Amp::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_Light(0);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTentacle_Amp::Ready_PartObjects()
{
	CMonster_HP::HPBAR_DESC hpDesc{};

	hpDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	hpDesc.pCombatCom = m_pCombatCom;
	/* For.UI */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::UI), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_HP"), &hpDesc)))
		return E_FAIL;


	return S_OK;
}

CTentacle_Amp* CTentacle_Amp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTentacle_Amp* pInstance = new CTentacle_Amp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTentacle_Amp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTentacle_Amp::Clone(void* pArg)
{
	CTentacle_Amp* pInstance = new CTentacle_Amp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTentacle_Amp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTentacle_Amp::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTarget);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCombatCom);
}
