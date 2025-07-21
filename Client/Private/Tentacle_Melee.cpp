#include "Tentacle_Melee.h"
#include "GameInstance.h"
#include "CombatStat.h"
#include "Monster_HP.h"

CTentacle_Melee::CTentacle_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster_Base{ pDevice, pContext }
{
}

CTentacle_Melee::CTentacle_Melee(const CTentacle_Melee& Prototype)
	: CMonster_Base{ Prototype }
{
}

HRESULT CTentacle_Melee::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTentacle_Melee::Initialize(void* pArg)
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

	m_pModelCom->Set_Animation(7, false);

	m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom, this);

	m_pColliderCom->Set_Active(false);

	
	m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Tentacle_Spawn_0.wav"), SOUND_MONSTER_DEATH, 1.f);

	return S_OK;
}

void CTentacle_Melee::Priority_Update(_float fTimeDelta)
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
	{
		Set_Dead();
		
		m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Tentacle_Death_0.wav"), SOUND_MONSTER_DEATH, 1.f);
	}

	__super::Priority_Update(fTimeDelta);
}

void CTentacle_Melee::Update(_float fTimeDelta)
{

	if (m_IsTargeted)
	{
		m_pTransformCom->LookAt(m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
	}

	m_IsFinished = m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));

	m_bDead = m_pCombatCom->Update(fTimeDelta);

	

	__super::Update(fTimeDelta);
}

void CTentacle_Melee::Late_Update(_float fTimeDelta)
{
	
	Select_State();

	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);

}

HRESULT CTentacle_Melee::Render()
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




#endif

	return S_OK;
}

HRESULT CTentacle_Melee::On_Collision(class CCollider* pCollider)
{
	if (m_IsColl)
		return S_OK;


	if (STATE_MAIN::ATTACK == m_eMainState && m_pModelCom->Get_Current_Anim_Ratio() > 0.3f)
	{
		
			m_pCombatCom->Attack(static_cast<CCombatStat*>(pCollider->Get_Owner()->Get_Component(TEXT("Com_Combat"))));
			m_IsColl = true;
		
	}

	return S_OK;
}

void CTentacle_Melee::Select_State()
{
	// 체력조건 만들어서 미리 보내버리기

	if (m_bDead)
	{
		m_eMainState = STATE_MAIN::DEAD;
		m_pModelCom->Set_Animation(6, false);
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
			m_pModelCom->Set_Animation(0, false);
			m_IsColl = false;
			m_pGameInstance->StopSound(SOUND_MONSTER_EFFECT);
			m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Tentacle_Slasher_Whoosh_0.wav"), SOUND_MONSTER_EFFECT, 1.f);
			// 공격 받으면 이제 idle에서만 바꾸기
			
			break;
		case Client::STATE_MAIN::ATTACK:
			m_eMainState = STATE_MAIN::IDLE;
			m_pModelCom->Set_Animation(5, false);
		
			break;
		case Client::STATE_MAIN::HIT:
			m_eMainState = STATE_MAIN::IDLE;
			m_pModelCom->Set_Animation(5, false);
			break;
		case Client::STATE_MAIN::SPWAN:
			m_eMainState = STATE_MAIN::IDLE;
			m_pModelCom->Set_Animation(5, false);
			m_pColliderCom->Set_Active(true);
			break;
		default:
			break;
		}
	}
}

HRESULT CTentacle_Melee::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Tentacle_Melee"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;



	CBounding_Sphere::SPHERE_DESC eDesc{};
	eDesc.fRadius = 10.f;
	eDesc.vCenter = { 0.f,2.f,0.f };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &eDesc)))
		return E_FAIL;

	CCombatStat::COMBAT_DESC eCombatDesc = {};
	eCombatDesc.iCurrentHp = 100;
	eCombatDesc.iMaxHp = 100;
	eCombatDesc.iDamage = 10;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eCombatDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTentacle_Melee::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CTentacle_Melee::Ready_PartObjects()
{
	CMonster_HP::HPBAR_DESC hpDesc{};

	hpDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();
	hpDesc.pCombatCom = m_pCombatCom;
	/* For.UI */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(PART_DEFAULT::UI), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_HP"), &hpDesc)))
		return E_FAIL;


	return S_OK;
}

CTentacle_Melee* CTentacle_Melee::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTentacle_Melee* pInstance = new CTentacle_Melee(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTentacle_Melee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTentacle_Melee::Clone(void* pArg)
{
	CTentacle_Melee* pInstance = new CTentacle_Melee(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTentacle_Melee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTentacle_Melee::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTarget);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCombatCom);
}
