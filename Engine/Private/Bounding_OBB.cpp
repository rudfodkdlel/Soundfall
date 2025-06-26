#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_OBB::Initialize(const CBounding::BOUNDING_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
	const OBB_DESC* pOBBDesc = static_cast<const OBB_DESC*>(pDesc);

	_float4		vQuaternion = {};

	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pOBBDesc->vRotation.x, pOBBDesc->vRotation.y, pOBBDesc->vRotation.z));


	m_pOriginalDesc = new BoundingOrientedBox(pOBBDesc->vCenter, pOBBDesc->vExtents, vQuaternion);
	m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{

	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_OBB::Intersect(CBounding* pTarget)
{
	_bool		isColl = { false };

	switch (pTarget->Get_Type())
	{
	case COLLIDER::AABB:
		isColl = Collision_OBB_To_AABB(pTarget);
		//isColl = m_pDesc->Intersects(*static_cast<CBounding_AABB*>(pTarget)->Get_Desc());
		break;
	case COLLIDER::OBB:
		isColl = Collision_OBB_To_OBB(pTarget);
		//isColl = m_pDesc->Intersects(*static_cast<CBounding_OBB*>(pTarget)->Get_Desc());
		
		break;
	case COLLIDER::SPHERE:
		isColl = Collision_OBB_To_Sphere(pTarget);
		//isColl = m_pDesc->Intersects(*static_cast<CBounding_Sphere*>(pTarget)->Get_Desc());
		break;

	}

	return isColl;
}

_bool CBounding_OBB::Collision_OBB_To_OBB(CBounding* pTarget)
{
	const BoundingOrientedBox* pDesc = static_cast<CBounding_OBB*>(pTarget)->Get_Desc();

	_vector CenterPos = XMLoadFloat3(&m_pDesc->Center);
	_vector ObbScale = XMLoadFloat3(&m_pDesc->Extents);
	_vector Rotation = XMLoadFloat4(&m_pDesc->Orientation);

	_matrix MatScale = XMMatrixScalingFromVector(ObbScale * 2.0f);
	_matrix MatRot = XMMatrixRotationQuaternion(Rotation);
	_matrix MatTras = XMMatrixTranslationFromVector(CenterPos);

	_matrix matWorld = MatScale * MatRot * MatTras;

	_vector vRight = XMVector3Normalize(matWorld.r[0]);
	_vector vLook = XMVector3Normalize(matWorld.r[2]);

	// 값을 가져와서 행렬로 만들자
	_vector TargetCenterPos = XMLoadFloat3(&pDesc->Center);
	_vector TargetObbScale = XMLoadFloat3(&pDesc->Extents);
	_vector TargetRotation = XMLoadFloat4(&pDesc->Orientation);

	_matrix TargetMatScale = XMMatrixScalingFromVector(TargetObbScale * 2.0f);
	_matrix TargetMatRot = XMMatrixRotationQuaternion(TargetRotation);
	_matrix TargetMatTras = XMMatrixTranslationFromVector(TargetCenterPos);

	// obb world를 만들어서 이걸로 이제 구를 가져오기
	_matrix TargetMatWorld = TargetMatScale * TargetMatRot * TargetMatTras;

	// 축을 구한다, x,z만
	_vector vTargetRight = XMVector3Normalize(TargetMatWorld.r[0]);
	_vector vTargetLook = XMVector3Normalize(TargetMatWorld.r[2]);

	// 계산할 축을 만듬


	// x,z 만 체크하려고
	auto Flatten = [](XMVECTOR v) {
		v.m128_f32[1] = 0.f; // y = 0
		return XMVector3Normalize(v);
		};



	vector<_vector> Axes = {
		Flatten(vRight), Flatten(vLook),
		Flatten(vTargetRight), Flatten(vTargetLook),
		 (Flatten(XMVector3Cross(vRight, vTargetRight))),
		(Flatten(XMVector3Cross(vRight, vTargetLook))),
		(Flatten(XMVector3Cross(vLook, vTargetRight))),
		(Flatten(XMVector3Cross(vLook, vTargetLook)))
	};

	float minOverlap = FLT_MAX;

	_vector mtvAxis = XMVectorZero();

	_vector centerDiff = XMLoadFloat3(&pDesc->Center) - XMLoadFloat3(&m_pDesc->Center);
	centerDiff.m128_f32[1] = 0.f;

	for (auto& Axis : Axes)
		Axis = XMVector3Normalize(Axis);


	for (auto& Axis : Axes)
	{
		// 축 크기가 너무 작으면 넘어가도록
		if (XMVector3LengthSq(Axis).m128_f32[0] < 1e-6f)
			continue;

		// 각 축에 투영시킨다
		float projAABB =
			fabsf(XMVectorGetX(XMVector3Dot(Axis, vRight))) * m_pDesc->Extents.x +
			fabsf(XMVectorGetX(XMVector3Dot(Axis, vLook))) * m_pDesc->Extents.z;

		float projOBB =
			fabsf(XMVectorGetX(XMVector3Dot(Axis, vTargetRight))) * pDesc->Extents.x +
			fabsf(XMVectorGetX(XMVector3Dot(Axis, vTargetLook))) * pDesc->Extents.z;


		float centerDist = fabsf(XMVectorGetX(XMVector3Dot(centerDiff, Axis)));

		float overlap = projAABB + projOBB - centerDist;

		if (overlap <= 0.f)
		{
			m_fDepth = 0.f;
			m_vMtv = {};
			return false;
		}

		if (overlap < minOverlap)
		{
			minOverlap = overlap;
			mtvAxis = Axis;

			// MTV 방향 조정 (centerDiff와 반대면 뒤집기)
			if (XMVectorGetX(XMVector3Dot(centerDiff, Axis)) < 0.f)
				mtvAxis = XMVectorNegate(Axis);
		}
	}



	m_fDepth = minOverlap;
	mtvAxis.m128_f32[1] = 0.f;
	XMStoreFloat3(&m_vMtv, -XMVector3Normalize(mtvAxis) * minOverlap);

	return true;
}

_bool CBounding_OBB::Collision_OBB_To_Sphere(CBounding* pTarget)
{
	_bool isColl = static_cast<CBounding_Sphere*>(pTarget)->Collision_Sphere_To_OBB(this);

	Set_Depth(pTarget->Get_Depth());
	// 반대 방향으로 넣어주기
	_float3 f3Mtv = pTarget->Get_Mtv();
	_vector vMtv = XMLoadFloat3(&f3Mtv);
	XMStoreFloat3(&m_vMtv, XMVectorNegate(vMtv));

	return isColl;
}

_bool CBounding_OBB::Collision_OBB_To_AABB(CBounding* pTarget)
{
	_bool isColl = static_cast<CBounding_AABB*>(pTarget)->Collision_AABB_To_OBB(this);

	Set_Depth(pTarget->Get_Depth());
	// 반대 방향으로 넣어주기
	_float3 f3Mtv = pTarget->Get_Mtv();
	_vector vMtv = XMLoadFloat3(&f3Mtv);
	XMStoreFloat3(&m_vMtv, XMVectorNegate(vMtv));

	return isColl;
}

#ifdef _DEBUG

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{

	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

#endif


_bool CBounding_OBB::Intersect_ToOBB(CBounding* pTarget)
{
	OBB_INFO	OBBDesc[2] = {
		Compute_OBB(),
		static_cast<CBounding_OBB*>(pTarget)->Compute_OBB()
	};

	_float		fDistance[3] = {};

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter),
				XMLoadFloat3(&OBBDesc[i].vAxisDir[j]))));

			fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[0]), XMLoadFloat3(&OBBDesc[i].vAxisDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[1]), XMLoadFloat3(&OBBDesc[i].vAxisDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[2]), XMLoadFloat3(&OBBDesc[i].vAxisDir[j]))));

			fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[0]), XMLoadFloat3(&OBBDesc[i].vAxisDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[1]), XMLoadFloat3(&OBBDesc[i].vAxisDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[2]), XMLoadFloat3(&OBBDesc[i].vAxisDir[j]))));


			if (fDistance[0] > fDistance[1] + fDistance[2])
				return false;
		}
	}



	return true;
}

CBounding_OBB::OBB_INFO CBounding_OBB::Compute_OBB()
{
	OBB_INFO		OBBDesc{};

	_float3			vPoints[8];

	m_pDesc->GetCorners(vPoints);

	OBBDesc.vCenter = m_pDesc->Center;

	XMStoreFloat3(&OBBDesc.vCenterDir[0], (XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterDir[1], (XMLoadFloat3(&vPoints[7]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterDir[2], (XMLoadFloat3(&vPoints[0]) - XMLoadFloat3(&vPoints[4])) * 0.5f);

	for (size_t i = 0; i < 3; i++)
		XMStoreFloat3(&OBBDesc.vAxisDir[i], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterDir[i])));

	return OBBDesc;
}

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CBounding::BOUNDING_DESC* pDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);
}
