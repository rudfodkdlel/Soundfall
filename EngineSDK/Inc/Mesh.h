#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	MESH_DESC Get_Desc() const
	{
		// 수정 필요
		MESH_DESC eDesc = {};
		strcpy_s(eDesc.szName, MAX_PATH, m_szName);
		eDesc.iMaterialIndex = m_iMaterialIndex;
		eDesc.iNumVertices = m_iNumVertices;
		eDesc.iNumIndices = m_iNumIndices;
		// 뼈 관련
		eDesc.iNumBones = m_iNumBones;
		eDesc.BoneIndices = m_BoneIndices;
		memcpy(eDesc.BoneMatrices, m_BoneMatrices, sizeof(_float4x4));
		eDesc.OffsetMatrices = m_OffsetMatrices;

		// vertex 
		eDesc.NonAnimMesh = m_NonAnimVertexes;
		eDesc.AnimMesh = m_AnimTertexes;
		// index
		eDesc.iIndices = m_iIndices;

		return eDesc;
	}

public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const MESH_DESC& eDesc, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_Bone_Matrices(class CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones);

private:
	_char			m_szName[MAX_PATH] = {};
	_uint			m_iMaterialIndex = {};

	/* 모델에 선언된 전체뼈(x) */
	/* 전체 뼈들 중, 이 메시에 영향을 주는 뼈들만골라서 모아놓은 컨테이너. */
	_uint			m_iNumBones = { };

	/* 전체를 기준으로 이 메시에 영향을 주는 뼈의 인덱스를 모아놓았다. */
	vector<_int>			m_BoneIndices;
	_float4x4				m_BoneMatrices[g_iMaxNumBones] = {};
	vector<_float4x4>		m_OffsetMatrices;

	// vertice들 저장해두기
	vector<VTXMESH>			m_NonAnimVertexes;
	vector<VTXANIMMESH>		m_AnimTertexes;

	// 삼각형 만들때 필요한거 저장용
	vector<_uint>			m_iIndices;

private:
	HRESULT Ready_NonAnim_Mesh(const MESH_DESC& eDesc, _fmatrix PreTransformMatrix);
	HRESULT Ready_Anim_Mesh(const MESH_DESC& eDesc, const vector<class CBone*>& Bones);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const MESH_DESC& eDesc, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END