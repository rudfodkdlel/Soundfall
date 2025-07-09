#include "VIBuffer_Mesh_Instance.h"

#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype)
    : CVIBuffer_Instance{ Prototype }
    , m_pVertexInstances{ Prototype.m_pVertexInstances }
{
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Prototype(const INSTANCE_DESC* pArg,  MESH_DESC meshDesc)
{

	const MESH_INSTANCE_DESC* pDesc = static_cast<const MESH_INSTANCE_DESC*>(pArg);

	m_iMaterialIndex = meshDesc.iMaterialIndex;

	m_iNumIndexPerInstance = meshDesc.iNumIndices;
	m_iVertexInstanceStride = sizeof(VTXMESH_INSTANCE);
	m_iNumInstance =  pDesc->iNumInstance;

	m_iNumVertexBuffers = 2;
	m_iNumVertices = meshDesc.iNumVertices;
	m_iVertexStride = sizeof(VTXMESH);
	m_iNumIndices = m_iNumIndexPerInstance;

	m_iIndexStride = sizeof(_uint);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEXBUFFER
	D3D11_BUFFER_DESC			VBBufferDesc{};
	VBBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VBBufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE*/0;
	VBBufferDesc.StructureByteStride = m_iVertexStride;
	VBBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA		VBInitialData{};

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = meshDesc.NonAnimMesh[i].vPosition;
		pVertices[i].vNormal = meshDesc.NonAnimMesh[i].vNormal;
		pVertices[i].vTangent = meshDesc.NonAnimMesh[i].vTangent;
		pVertices[i].vTexcoord = meshDesc.NonAnimMesh[i].vTexcoord;
	}


	for (_uint i = 0; i < m_iNumVertices; ++i)
		m_pVertexPositions[i] = meshDesc.NonAnimMesh[i].vPosition;

	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBBufferDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion 

#pragma region INDEXBUFFER

	D3D11_BUFFER_DESC			IBBufferDesc{};
	IBBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IBBufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE*/0;
	IBBufferDesc.StructureByteStride = m_iIndexStride;
	IBBufferDesc.MiscFlags = 0;

	vector<_uint> indices(meshDesc.iNumIndices);
	for (_uint i = 0; i < meshDesc.iNumIndices; ++i)
		indices[i] = meshDesc.iIndices[i];

	
	D3D11_SUBRESOURCE_DATA		IBInitialData{};
	IBInitialData.pSysMem = indices.data();

	if (FAILED(m_pDevice->CreateBuffer(&IBBufferDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	
#pragma endregion 

#pragma region INSTANCEBUFFER
	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iVertexInstanceStride;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.StructureByteStride = m_iVertexInstanceStride;
	m_VBInstanceDesc.MiscFlags = 0;

	m_pVertexInstances = new VTXMESH_INSTANCE[m_iNumInstance];


	for (size_t i = 0; i < m_iNumInstance; i++)
	{
	
		_float	fSize = m_pGameInstance->Compute_Random(pDesc->vSize.x, pDesc->vSize.y);

		m_pVertexInstances[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
		m_pVertexInstances[i].vUp = _float4(0.f, fSize, 0.f, 0.f);
		m_pVertexInstances[i].vLook = _float4(0.f, 0.f, fSize, 0.f);


		m_pVertexInstances[i].vTranslation = _float4(
			m_pGameInstance->Compute_Random(pDesc->vCenter.x - pDesc->vRange.x * 0.5f, pDesc->vCenter.x + pDesc->vRange.x * 0.5f),
			m_pGameInstance->Compute_Random(pDesc->vCenter.y - pDesc->vRange.y * 0.5f, pDesc->vCenter.y + pDesc->vRange.y * 0.5f),
			m_pGameInstance->Compute_Random(pDesc->vCenter.z - pDesc->vRange.z * 0.5f, pDesc->vCenter.z + pDesc->vRange.z * 0.5f),
			1.f
		);

	
	}

	m_VBInstanceSubresourceData.pSysMem = m_pVertexInstances;



#pragma endregion 




	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Initialize(void* pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, &m_VBInstanceSubresourceData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Mesh_Instance* CVIBuffer_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc, MESH_DESC meshDesc)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc, meshDesc)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Mesh_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Mesh_Instance::Clone(void* pArg)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Mesh_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Mesh_Instance::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pVertexInstances);
}
