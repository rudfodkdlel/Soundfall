#include "Weapon_Base.h"
#include "GameInstance.h"
#include "Model.h"

CWeapon_Base::CWeapon_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon_Base::CWeapon_Base(const CWeapon_Base& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CWeapon_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Base::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void CWeapon_Base::Priority_Update(_float fTimeDelta)
{
}

void CWeapon_Base::Update(_float fTimeDelta)
{
	_matrix		BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix)
	);
	
	if(nullptr != m_pColliderCom)
		m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CWeapon_Base::Late_Update(_float fTimeDelta)
{


	if(m_IsActive)
		m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CWeapon_Base::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, 1, 0)))
			return E_FAIL;

		// alpha ¾È¾²´Â	
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		//if (FAILED(m_pShaderCom->Begin(0)))
		//	return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	if(nullptr != m_pColliderCom && m_pColliderCom->Get_Active())
		m_pColliderCom->Render();

#endif

	return S_OK;

}

void CWeapon_Base::Check_Timing()
{
	if (m_pGameInstance->Get_Timing() < 0.15f)
	{
		m_IsPerfect = true;
	}
	else
	{
		m_IsPerfect = false;
	}
}

HRESULT CWeapon_Base::Ready_Components()
{
	return S_OK;
}

HRESULT CWeapon_Base::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
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

void CWeapon_Base::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
