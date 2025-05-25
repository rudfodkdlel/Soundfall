#include "Model.h"

#include "Component.h"
#include "Animation.h"
#include "Material.h"
#include "Mesh.h"
#include "Bone.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
	, m_iNumMeshes{ Prototype.m_iNumMeshes }
	, m_Meshes{ Prototype.m_Meshes }
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials{ Prototype.m_Materials }
	, m_eType{ Prototype.m_eType }
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
	//, m_Bones { Prototype.m_Bones }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	// , m_Animations { Prototype.m_Animations }
{
	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& pPrototypeAnim : Prototype.m_Animations)
		m_Animations.push_back(pPrototypeAnim->Clone());

}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _int eType, _uint iTextureIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_ShaderResource(pShader, pConstantName, eType, iTextureIndex);
}



HRESULT CModel::Bind_Bone_Matrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_Bone_Matrices(pShader, pConstantName, m_Bones);
}

_float4 CModel::Compute_PickedPosition(const _float4x4* pWorldMatrixInverse)
{
	_float4 vPos = {};
	for (auto& meshes : m_Meshes)
	{
		
		vPos = meshes->Compute_PickedPosition(pWorldMatrixInverse);
		if (vPos.w != 0)
			break;
	}

	return vPos;
}

HRESULT CModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	m_eType = eType;

	Load_Binary(pModelFilePath);

	 m_iNumAnimations = m_eModelDesc.iNumAnimations;
	 m_iNumMaterials = m_eModelDesc.iNumMaterials;
	 m_iNumMeshes = m_eModelDesc.iNumMeshes;

	 if (FAILED(Ready_Bones(m_BoneDescs)))
	 	return E_FAIL;
	 if (FAILED(Ready_Meshes(m_MeshDescs)))
	 	return E_FAIL;
	 if (FAILED(Ready_Materials(m_MaterialDescs, pModelFilePath)))
	 	return E_FAIL;
	 if (FAILED(Ready_Animations(m_AnimDescs)))
	 	return E_FAIL;

	 for (auto& p : m_MeshDescs)
	 {
		 p->iIndices.clear();
		 p->AnimMesh.clear();
		 p->NonAnimMesh.clear();
		 p->OffsetMatrices.clear();
		 Safe_Delete(p);
	 }
	 m_MeshDescs.clear();

	 for (auto& p : m_MaterialDescs)
	 {
		 p->TexturePaths->clear();

		 Safe_Delete(p);
	 }
	 m_MaterialDescs.clear();

	 for (auto& p : m_AnimDescs) 
	 {
		 for (auto& channel : p->Channels)
		 {
			 channel.KeyFrames.clear();
		 }
		 p->Channels.clear();
		 Safe_Delete(p);
	 }
	 m_AnimDescs.clear();

	 for (auto& p : m_BoneDescs)
		Safe_Delete(p);
	 m_BoneDescs.clear();

 	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
	_bool		isFinished = { false };
	
	// 다음 애니메이션이 같으면
	if (m_iCurrentAnimIndex == m_iNextAnimIndex)
	{
		/* 1. 현재 애니메이션에 맞는 뼈의 상태를 읽어와서 뼈의 TrnasformationMatrix를 갱신해준다. */
		isFinished = m_Animations[m_iCurrentAnimIndex]->Update_Bones(fTimeDelta, m_Bones, m_isLoop);

		/* 2. 전체 뼐르 순회하면서 뼈들의 ColmbinedTransformationMatixf를 부모에서부터 자식으로 갱신해주낟. */
		for (auto& pBone : m_Bones)
		{
			pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
		}

	}
	// 바뀐 상태가 아니면 이제 다음 애니메이션으로 바꿀 준비를 한다
	else
	{
		Change_Animation(fTimeDelta, m_iNextAnimIndex);
	}
	

	/*XMMatrixDecompose()*/

	return isFinished;
}

void CModel::Change_Animation(_float fTimeDelta, _uint iIndex)
{
	
	// 같은 곳으로 이동하면 안바꾸기
	if (iIndex == m_iCurrentAnimIndex)
	{
		return;
	}

	m_fChangeDuration += fTimeDelta;
	_float fDuration = 0.2f;
	_float fRatio = min(m_fChangeDuration / fDuration, 1.f);
	

	if (fRatio < 1.f)
	{
		vector< _vector> vCurPosvector;
		vector< _vector> vCurScaleVector;
		vector< _vector> vCurRotationVector;

		vector< _vector> vNextPosvector;
		vector< _vector> vNextScaleVector;
		vector< _vector> vNextRotationVector;

		_matrix			TransformationMatrix{};

		// 현재 부분에 대한거를 가져옴
		for (int i = 0; i < m_Bones.size(); ++i)
		{
			_vector vPos, vScale, vRotation;

			XMMatrixDecompose(&vScale, &vRotation, &vPos, XMLoadFloat4x4(m_Bones[i]->Get_TransformationMatrix()));
			vCurPosvector.push_back(vPos);
			vCurScaleVector.push_back(vScale);
			vCurRotationVector.push_back(vRotation);
		}

		// 다음 부분에 대한거를 가져오기

		// 다음 애니메이션의 첫번째 키 프레임 정보로 뼈를 다시 세팅 하고 값 저장
		m_Animations[iIndex]->Update_Bones(0.f, m_Bones, m_isLoop);

		for (int i = 0; i < m_Bones.size(); ++i)
		{
			_vector vPos, vScale, vRotation;

			XMMatrixDecompose(&vScale, &vRotation, &vPos, XMLoadFloat4x4(m_Bones[i]->Get_TransformationMatrix()));
			vNextPosvector.push_back(vPos);
			vNextScaleVector.push_back(vScale);
			vNextRotationVector.push_back(vRotation);
		}

		for (int i = 0; i < m_Bones.size(); ++i)
		{
			_vector vPos, vScale, vRotation;


			vScale = XMVectorLerp(vCurScaleVector[i], vNextScaleVector[i], fRatio);
			vRotation = XMQuaternionSlerp(vCurRotationVector[i], vNextRotationVector[i], fRatio);
			vPos = XMVectorLerp(vCurPosvector[i], vNextPosvector[i], fRatio);

			// TransformationMatrix = XMMatrixScaling() * XMMatrixRotationQuaternion() * XMMatrixTranslation();
			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPos);
			m_Bones[i]->Set_TransformationMatrix(TransformationMatrix);
		}

		// 보간 후 부모 행렬 바뀐 걸 갱신해준다
		for (auto& pBone : m_Bones)
		{
			pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
		}

	}
	else 
	{
		// 보간이 끝나면 현재 index를 바꾸기
		m_fChangeDuration = 0.f;
		m_iCurrentAnimIndex = iIndex;
	}

	
}

void CModel::Load_Binary(const _char* pModelFilePath)
{
	

	ifstream fin(pModelFilePath, ios::binary);
	if (!fin)
		return;

	// modeldesc
	fin.read(reinterpret_cast<char*>(&m_eModelDesc), sizeof(MODEL_DESC));

	// bone
	size_t numBones = 0;
	fin.read(reinterpret_cast<char*>(&numBones), sizeof(numBones));
	for (size_t i = 0; i < numBones; ++i)
	{
		BONE_DESC* pBone = new BONE_DESC;
		fin.read(reinterpret_cast<char*>(pBone), sizeof(BONE_DESC));
		m_BoneDescs.push_back(pBone);
	}

	// mesh
	size_t meshCount = 0;
	fin.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));
	for (size_t i = 0; i < meshCount; ++i)
	{
		MESH_DESC* pMesh = new MESH_DESC;
		fin.read(reinterpret_cast<char*>(pMesh->szName), sizeof(char) * MAX_PATH);
		fin.read(reinterpret_cast<char*>(&pMesh->iMaterialIndex), sizeof(_uint));
		fin.read(reinterpret_cast<char*>(&pMesh->iNumVertices), sizeof(_uint));
		fin.read(reinterpret_cast<char*>(&pMesh->iNumIndices), sizeof(_uint));
		fin.read(reinterpret_cast<char*>(&pMesh->iNumBones), sizeof(_uint));
		fin.read(reinterpret_cast<char*>(pMesh->BoneMatrices), sizeof(XMFLOAT4X4) * 512);

		size_t boneIndices = 0;
		fin.read(reinterpret_cast<char*>(&boneIndices), sizeof(boneIndices));
		pMesh->BoneIndices.resize(boneIndices);
		if (boneIndices > 0)
			fin.read(reinterpret_cast<char*>(pMesh->BoneIndices.data()), sizeof(int) * boneIndices);

		size_t offsetCount = 0;
		fin.read(reinterpret_cast<char*>(&offsetCount), sizeof(offsetCount));
		pMesh->OffsetMatrices.resize(offsetCount);
		if (offsetCount > 0)
			fin.read(reinterpret_cast<char*>(pMesh->OffsetMatrices.data()), sizeof(XMFLOAT4X4) * offsetCount);

		size_t countVTXMESH = 0;
		fin.read(reinterpret_cast<char*>(&countVTXMESH), sizeof(countVTXMESH));
		pMesh->NonAnimMesh.resize(countVTXMESH);
		if (countVTXMESH > 0)
			fin.read(reinterpret_cast<char*>(pMesh->NonAnimMesh.data()), sizeof(VTXMESH) * countVTXMESH);

		size_t countVTXANIMMESH = 0;
		fin.read(reinterpret_cast<char*>(&countVTXANIMMESH), sizeof(countVTXANIMMESH));
		pMesh->AnimMesh.resize(countVTXANIMMESH);
		if (countVTXANIMMESH > 0)
			fin.read(reinterpret_cast<char*>(pMesh->AnimMesh.data()), sizeof(VTXANIMMESH) * countVTXANIMMESH);

		size_t countIndices = 0;
		fin.read(reinterpret_cast<char*>(&countIndices), sizeof(countIndices));
		pMesh->iIndices.resize(countIndices);
		if (countIndices > 0)
			fin.read(reinterpret_cast<char*>(pMesh->iIndices.data()), sizeof(_uint) * countIndices);

		m_MeshDescs.push_back(pMesh);
	}

	// material
	size_t materialCount = 0;
	fin.read(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));
	for (size_t i = 0; i < materialCount; ++i)
	{
		MATERIAL_DESC* pMaterial = new MATERIAL_DESC;
		fin.read(reinterpret_cast<char*>(pMaterial->iNumSRVs), sizeof(unsigned int) * 27);

		for (int j = 0; j < 27; ++j)
		{
			size_t vecSize = 0;
			fin.read(reinterpret_cast<char*>(&vecSize), sizeof(vecSize));
			pMaterial->TexturePaths[j].resize(vecSize);
			if (vecSize > 0)
				fin.read(reinterpret_cast<char*>(pMaterial->TexturePaths[j].data()), sizeof(MATERIAL_PATH_DESC) * vecSize);
		}

		m_MaterialDescs.push_back(pMaterial);
	}

	// animation
	size_t animationCount = 0;
	fin.read(reinterpret_cast<char*>(&animationCount), sizeof(animationCount));
	for (size_t i = 0; i < animationCount; ++i)
	{
		ANIM_DESC* pAnim = new ANIM_DESC;
		fin.read(reinterpret_cast<char*>(&pAnim->iNumChannels), sizeof(_uint));
		fin.read(reinterpret_cast<char*>(&pAnim->fTickPerSecond), sizeof(float));
		fin.read(reinterpret_cast<char*>(&pAnim->fDuration), sizeof(float));

		size_t channelCount = 0;
		fin.read(reinterpret_cast<char*>(&channelCount), sizeof(channelCount));
		pAnim->Channels.resize(channelCount);
		for (size_t j = 0; j < channelCount; ++j)
		{
			fin.read(reinterpret_cast<char*>(pAnim->Channels[j].szName), sizeof(char) * MAX_PATH);
			fin.read(reinterpret_cast<char*>(&pAnim->Channels[j].iNumKeyFrames), sizeof(_uint));
			fin.read(reinterpret_cast<char*>(&pAnim->Channels[j].iBoneIndex), sizeof(_uint));

			size_t keyFrameCount = 0;
			fin.read(reinterpret_cast<char*>(&keyFrameCount), sizeof(keyFrameCount));
			pAnim->Channels[j].KeyFrames.resize(keyFrameCount);
			if (keyFrameCount > 0)
				fin.read(reinterpret_cast<char*>(pAnim->Channels[j].KeyFrames.data()), sizeof(KEYFRAME) * keyFrameCount);
		}

		m_AnimDescs.push_back(pAnim);
	}

	fin.close();
}

HRESULT CModel::Ready_Bones(const vector<BONE_DESC*>& eDesc)
{
	for (int i = 0; i < m_eModelDesc.iBoneNums; ++i)
	{
		CBone* pBone = CBone::Create(*eDesc[i]);
		if (nullptr == pBone)
			return E_FAIL;
		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes(const vector<MESH_DESC*>& eDesc)
{

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, *eDesc[i], m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;


}

HRESULT CModel::Ready_Materials(const vector<MATERIAL_DESC*>& eDesc, const _char* pModelFilePath)
{

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, *eDesc[i], pModelFilePath);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_Materials.push_back(pMaterial);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations(const vector<ANIM_DESC*>& eDesc)
{
	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(*eDesc[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& pMaterial : m_Materials)
		Safe_Release(pMaterial);
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

}
