#include "Mesh.h"

#include "Bone.h"
#include "Shader.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(MODEL eType, const MESH_DESC& eDesc, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix)
{

	m_iNumBones = eDesc.iNumBones;

	strcpy_s(m_szName, eDesc.szName);
	m_iMaterialIndex = eDesc.iMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = eDesc.iNumVertices;
	m_iNumIndices = eDesc.iNumIndices;
	m_iIndexStride = sizeof(_uint);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	HRESULT hr = eType == MODEL::NONANIM ? Ready_NonAnim_Mesh(eDesc, PreTransformMatrix) : Ready_Anim_Mesh(eDesc, Bones);

	if (FAILED(hr))
		return E_FAIL;


	D3D11_BUFFER_DESC			IBBufferDesc{};
	IBBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IBBufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE*/0;
	IBBufferDesc.StructureByteStride = m_iIndexStride;
	IBBufferDesc.MiscFlags = 0;

	m_iNumPritimive = m_iNumIndices / 3;


	m_iIndices.resize(eDesc.iIndices.size());
	m_iIndices = eDesc.iIndices;
	m_pIndices = new _uint[m_iNumIndices];

	D3D11_SUBRESOURCE_DATA		IBInitialData{};
	IBInitialData.pSysMem = m_iIndices.data();

	if (FAILED(m_pDevice->CreateBuffer(&IBBufferDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	memcpy(m_pIndices, m_iIndices.data(), sizeof(_uint) * m_iNumIndices);

	




	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}




HRESULT CMesh::Ready_NonAnim_Mesh(const MESH_DESC& eDesc, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);
	D3D11_BUFFER_DESC			VBBufferDesc{};
	VBBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VBBufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE*/0;
	VBBufferDesc.StructureByteStride = m_iVertexStride;
	VBBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA		VBInitialData{};

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	m_NonAnimVertexes.resize(m_iNumVertices);
	m_NonAnimVertexes = eDesc.NonAnimMesh;

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&m_NonAnimVertexes[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&m_NonAnimVertexes[i].vPosition), PreTransformMatrix));
		XMStoreFloat3(&m_NonAnimVertexes[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&m_NonAnimVertexes[i].vNormal), PreTransformMatrix));
		XMStoreFloat2(&m_NonAnimVertexes[i].vTexcoord, XMVector2TransformNormal(XMLoadFloat2(&m_NonAnimVertexes[i].vTexcoord), PreTransformMatrix));
	}


	for (_uint i = 0; i < m_iNumVertices; ++i)
		m_pVertexPositions[i] = m_NonAnimVertexes[i].vPosition;


	VBInitialData.pSysMem = m_NonAnimVertexes.data();

	if (FAILED(m_pDevice->CreateBuffer(&VBBufferDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CMesh::Ready_Anim_Mesh(const MESH_DESC& eDesc, const vector<class CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);
	D3D11_BUFFER_DESC			VBBufferDesc{};
	VBBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VBBufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE*/0;
	VBBufferDesc.StructureByteStride = m_iVertexStride;
	VBBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA		VBInitialData{};

	m_AnimTertexes.resize(m_iNumVertices);
	m_AnimTertexes = eDesc.AnimMesh;

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	m_BoneIndices.resize(eDesc.BoneIndices.size());
	m_BoneIndices = eDesc.BoneIndices;

	m_OffsetMatrices.resize(eDesc.OffsetMatrices.size());
	m_OffsetMatrices = eDesc.OffsetMatrices;

	for (_uint i = 0; i < m_iNumVertices; ++i)
		m_pVertexPositions[i] = m_AnimTertexes[i].vPosition;


	VBInitialData.pSysMem = m_AnimTertexes.data();

	if (FAILED(m_pDevice->CreateBuffer(&VBBufferDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	m_AnimTertexes.clear();

	return S_OK;
}

HRESULT CMesh::Bind_Bone_Matrices(CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * g_iMaxNumBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i],
			XMLoadFloat4x4(&m_OffsetMatrices[i]) *
			XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix()));
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const MESH_DESC& eDesc, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, eDesc, Bones, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	m_iIndices.clear();
	m_NonAnimVertexes.clear();

}
