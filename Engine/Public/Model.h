#pragma once

#include "Component.h"
#include "Animation.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
protected:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_uint Get_NumAnimations() const {
		return m_iNumAnimations;
	}


	void Set_Animation(_uint iIndex, _bool isLoop = true) {
		m_iNextAnimIndex = iIndex;
		m_isLoop = isLoop;
	}

	const _float4x4* Get_BoneMatrix(const _char* pBoneName) const;

	_float Get_Current_Anim_Ratio() { return m_Animations[m_iCurrentAnimIndex]->Get_Current_Ratio(); }

public:

	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _int eType, _uint iTextureIndex = 0);
	HRESULT Bind_Bone_Matrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	_float4 Compute_PickedPosition(const _float4x4* pWorldMatrixInverse);

public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	_bool Play_Animation(_float fTimeDelta);

	// 다른 애니메이션으로 이동하면서 보간 하도록
	void Change_Animation(_float fTimeDelta, _uint iIndex);

	void Load_Binary(const _char* pModelFilePath);

private:

	// 파일에 담아둘 변수들을 구조체 형태로 저장해둔다
	MODEL_DESC					m_eModelDesc = { };
	vector<MESH_DESC*>			m_MeshDescs;
	vector<MATERIAL_DESC*>		m_MaterialDescs;
	vector<ANIM_DESC*>			m_AnimDescs;
	vector<BONE_DESC*>           m_BoneDescs;

	// 만들때 내가 넣어줘야 되는 값들
	MODEL						m_eType = {};
	_float4x4					m_PreTransformMatrix = {};

	// 파일로 읽어오면 되는것들
	_uint						m_iNumMeshes = {};
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = {};
	vector<class CMaterial*>	m_Materials;

	vector<class CBone*>		m_Bones;

	_bool						m_isLoop{};
	_uint						m_iCurrentAnimIndex = { 0 };
	_uint						m_iNextAnimIndex = { 0 };
	_uint						m_iNumAnimations = {};
	vector<CAnimation*>	m_Animations;

	_float						m_fChangeDuration = {};

public:

	HRESULT Ready_Bones(const vector<BONE_DESC*>& eDesc);
	HRESULT Ready_Meshes(const vector<MESH_DESC*>& eDesc);
	HRESULT Ready_Materials(const vector<MATERIAL_DESC*>& eDesc, const _char* pModelFilePath);
	HRESULT Ready_Animations(const vector<ANIM_DESC*>& eDesc);


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END