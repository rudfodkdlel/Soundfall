#include "Body_Sniper.h"
#include "GameInstance.h"
#include "Model.h"
#include "Object_State_Spawn.h"
#include "Observer_Animation.h"
#include "CombatStat.h"

CBody_Sniper::CBody_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Sniper::CBody_Sniper(const CBody_Sniper& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CBody_Sniper::Get_SocketMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrix(pBoneName);
}

_float CBody_Sniper::Get_AnimRatio()
{
	return  m_pModelCom->Get_Current_Anim_Ratio(); 
}


HRESULT CBody_Sniper::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Sniper::Initialize(void* pArg)
{
	NORMAL_BODY_DESC* pDesc = static_cast<NORMAL_BODY_DESC*>(pArg);
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pCombatCom = pDesc->pCombatCom;
	Safe_AddRef(m_pCombatCom);

	m_Components.emplace(TEXT("Com_Combat"), m_pCombatCom);

	m_pModelCom->Set_Animation(15, false);

	m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom, this);
	return S_OK;
}

void CBody_Sniper::Priority_Update(_float fTimeDelta)
{
}

void CBody_Sniper::Update(_float fTimeDelta)
{
	m_IsFinished = m_pModelCom->Play_Animation(fTimeDelta);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CBody_Sniper::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CBody_Sniper::Render()
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

HRESULT CBody_Sniper::On_Collision(CCollider* pCollider)
{
	if (CG_WEAPON_MONSTER == pCollider->Get_Group())
		return S_OK;

	m_pushVectors.push_back(m_pColliderCom->Calc_PushVector(pCollider));
	return S_OK;
}

HRESULT CBody_Sniper::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Sniper"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_OBB::OBB_DESC	OBBDesc{};
	OBBDesc.vExtents = _float3(1.5f, 2.f, 1.f);
	OBBDesc.vCenter = _float3(0.0f, OBBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Sniper::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	return S_OK;
}

CBody_Sniper* CBody_Sniper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Sniper* pInstance = new CBody_Sniper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Sniper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Sniper::Clone(void* pArg)
{
	CBody_Sniper* pInstance = new CBody_Sniper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Sniper");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CBody_Sniper::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pCombatCom);
	Safe_Release(m_pColliderCom);
}

