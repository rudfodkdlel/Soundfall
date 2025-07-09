#include "VIBuffer_Model_Instance.h"
#include "VIBuffer_Mesh_Instance.h"
#include "GameInstance.h"
#include "Material.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"

CVIBuffer_Model_Instance::CVIBuffer_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Model_Instance::CVIBuffer_Model_Instance(const CVIBuffer_Model_Instance& Prototype)
	: CVIBuffer_Instance{ Prototype }
	, m_MeshIntances{Prototype.m_MeshIntances}
	, m_Materials{Prototype.m_Materials}
{
}

HRESULT CVIBuffer_Model_Instance::Initialize_Prototype(const INSTANCE_DESC* pArg)
{
	const Model_INSTANCE_DESC* pDesc = static_cast<const Model_INSTANCE_DESC*>(pArg);
	// 만들어놓은 모델 정보를 가져와서 값을 채우자
	auto iter = m_pGameInstance->Get_Prototypes()->find(pDesc->strModelDesc);

	if (m_pGameInstance->Get_Prototypes()->end() == iter)
		return E_FAIL;

	CModel* pModel = static_cast<CModel*>(iter->second);

	if (nullptr == pModel)
		return E_FAIL;

	m_Materials = pModel->Get_Material();


	// 모델이 가지고 있던 메쉬 수 만큼 돌면서 각 매쉬마다 instancing 할 수 있게
	for (auto& pMesh : pModel->Get_Mesh())
	{
 		CVIBuffer_Mesh_Instance* pTemp = CVIBuffer_Mesh_Instance::Create(m_pDevice, m_pContext, pDesc, pMesh->Get_Desc());
		
		m_MeshIntances.push_back(pTemp);
	}



	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Initialize(void* pArg)
{
	for (auto& pMesh : m_MeshIntances)
		pMesh->Initialize(pArg);
	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Bind_Buffers()
{
	for (auto& pMesh : m_MeshIntances)
		pMesh->Bind_Buffers();

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Render()
{
	for (auto& pMesh : m_MeshIntances)
	{
		pMesh->Bind_Buffers();
		pMesh->Render();
	}
		
	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _int eType, _uint iTextureIndex)
{
	if (iMeshIndex >= m_MeshIntances.size())
		return E_FAIL;

	_uint		iMaterialIndex = m_MeshIntances[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_MeshIntances.size())
		return E_FAIL;

	HRESULT hr = m_Materials[iMaterialIndex]->Bind_ShaderResource(pShader, pConstantName, eType, iTextureIndex);

	return hr;
}



CVIBuffer_Model_Instance* CVIBuffer_Model_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pArg)
{
	CVIBuffer_Model_Instance* pInstance = new CVIBuffer_Model_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Model_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Model_Instance::Clone(void* pArg)
{
	CVIBuffer_Model_Instance* pInstance = new CVIBuffer_Model_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Model_Instance::Free()
{
	__super::Free();

	for (auto& pMesh : m_MeshIntances)
		Safe_Release(pMesh);

}
