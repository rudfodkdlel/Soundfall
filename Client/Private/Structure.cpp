#pragma once
#include "Structure.h"
#include "GameInstance.h"
#include "Model.h"

CStructure::CStructure(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CStructure::CStructure(const CStructure& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CStructure::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStructure::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};

	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	lstrcpy(Desc.szName, TEXT("Monster"));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 1. 서로 다른 애니메이션을 셋팅했음에도 같은 동작이 재생된다. : 뼈가 공유되기때문에. */
	/* 2. 같은 애니메이션을 셋했다면 재생속도가 빨라진다. : */

	//m_pModelCom->Set_Animation(0, true);


	return S_OK;
}

void CStructure::Priority_Update(_float fTimeDelta)
{

}

void CStructure::Update(_float fTimeDelta)
{


	//bool a = m_pModelCom->Play_Animation(fTimeDelta);


}

void CStructure::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CStructure::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, 1, 0)))
			return E_FAIL;

		m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);

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

HRESULT CStructure::Ready_Components()
{
	// non anim
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Slope_0"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;



	return S_OK;
}

CStructure* CStructure::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStructure* pInstance = new CStructure(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStructure");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStructure::Clone(void* pArg)
{
	CStructure* pInstance = new CStructure(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStructure");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStructure::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
