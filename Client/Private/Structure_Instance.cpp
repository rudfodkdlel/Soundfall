#include "Structure_Instance.h"
#include "GameInstance.h"

CStructure_Instance::CStructure_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{pDevice, pContext}
{
}

CStructure_Instance::CStructure_Instance(const CStructure_Instance& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CStructure_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStructure_Instance::Initialize(void* pArg)
{
	STRUCTURE_INSTANCE_DESC* Desc = static_cast<STRUCTURE_INSTANCE_DESC*>(pArg);




	if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

	m_strProtoTag = TEXT("Prototype_GameObject_Structure_Instance");


	if (FAILED(Ready_Components(Desc)))
		return E_FAIL;

	/* 1. 서로 다른 애니메이션을 셋팅했음에도 같은 동작이 재생된다. : 뼈가 공유되기때문에. */
	/* 2. 같은 애니메이션을 셋했다면 재생속도가 빨라진다. : */

	//m_pModelCom->Set_Animation(0, true);


	return S_OK;
}

void CStructure_Instance::Priority_Update(_float fTimeDelta)
{
}

void CStructure_Instance::Update(_float fTimeDelta)
{

}

void CStructure_Instance::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CStructure_Instance::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", 0, 1, 0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Material(m_pShaderCom, "g_NormalTexture", 0, 6, 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	


	return S_OK;
}

HRESULT CStructure_Instance::Ready_Components(STRUCTURE_INSTANCE_DESC* eDesc)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMeshInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), eDesc->strBuffertag,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	

	return S_OK;
}

CStructure_Instance* CStructure_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStructure_Instance* pInstance = new CStructure_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStructure_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStructure_Instance::Clone(void* pArg)
{
	CStructure_Instance* pInstance = new CStructure_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStructure_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStructure_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
