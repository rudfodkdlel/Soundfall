#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const BONE_DESC& eDesc)
{
	strcpy_s(m_szName, eDesc.szName);

	memcpy(&m_TransformationMatrix, &eDesc.Transformation, sizeof(_float4x4));

	//XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = eDesc.iParentBoneIndex;

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);

	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));

}

CBone* CBone::Create(const BONE_DESC& eDesc)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(eDesc)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();

}
