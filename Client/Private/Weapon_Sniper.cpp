#include "Weapon_Sniper.h"
#include "GameInstance.h"
#include "Model.h"

CWeapon_Sniper::CWeapon_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice, pContext }
{
}

CWeapon_Sniper::CWeapon_Sniper(const CWeapon_Sniper& Prototype)
    :CPartObject{ Prototype }
{
}

HRESULT CWeapon_Sniper::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon_Sniper::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	m_eMainState = pDesc->eMainState;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 보고 바꾸기
	m_pTransformCom->Scaling(2.f, 2.f, 2.f);
	//_vector vAxis = { 1.f,0.f,0.f,0.f };
	//m_pTransformCom->Rotation(vAxis, XMConvertToRadians(-90.f));


	return S_OK;
}

void CWeapon_Sniper::Priority_Update(_float fTimeDelta)
{
	
}

void CWeapon_Sniper::Update(_float fTimeDelta)
{
	_matrix		BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);


	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix)
	);
}

void CWeapon_Sniper::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CWeapon_Sniper::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, 1, 0)))
			return E_FAIL;

		// alpha 안쓰는	
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		//if (FAILED(m_pShaderCom->Begin(0)))
		//	return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CWeapon_Sniper::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Sniper_Weapon"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Sniper::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	return S_OK;
}

CWeapon_Sniper* CWeapon_Sniper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Sniper* pInstance = new CWeapon_Sniper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Sniper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Sniper::Clone(void* pArg)
{
	CWeapon_Sniper* pInstance = new CWeapon_Sniper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Sniper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Sniper::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

}


