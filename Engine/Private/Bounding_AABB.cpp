#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_AABB::Initialize(const CBounding::BOUNDING_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	const AABB_DESC* pAABBDesc = static_cast<const AABB_DESC*>(pDesc);

	m_pOriginalDesc = new BoundingBox(pAABBDesc->vCenter, pAABBDesc->vExtents);
	m_pDesc = new BoundingBox(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix			TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]);

	m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

_bool CBounding_AABB::Intersect(CBounding* pTarget)
{
	_bool		isColl = { false };

	switch (pTarget->Get_Type())
	{
	case COLLIDER::AABB:
		isColl = Collision_AABB_To_AABB(pTarget);
		//isColl = m_pDesc->Intersects(*static_cast<CBounding_AABB*>(pTarget)->Get_Desc());
		
		break;
	case COLLIDER::OBB:
		isColl = Collision_AABB_To_OBB(pTarget);
		//isColl = m_pDesc->Intersects(*static_cast<CBounding_OBB*>(pTarget)->Get_Desc());
		break;
	case COLLIDER::SPHERE:
		isColl = Collision_AABB_To_Sphere(pTarget);
		//isColl = m_pDesc->Intersects(*static_cast<CBounding_Sphere*>(pTarget)->Get_Desc());
		break;

	}

	return isColl;
}

_bool CBounding_AABB::Collision_AABB_To_AABB(CBounding* pTarget)
{
	const BoundingBox* pDesc = static_cast<CBounding_AABB*>(pTarget)->Get_Desc();

	// x,z 만 비교
	_float2 fDistCenterToCenter = { fabsf(m_pDesc->Center.x - pDesc->Center.x), fabsf(m_pDesc->Center.z - pDesc->Center.z) };

	// 중점 사이 거리가 축 합보다 작으면 충돌 x
	if (fDistCenterToCenter.x > m_pDesc->Extents.x + pDesc->Extents.x ||
		fDistCenterToCenter.y > m_pDesc->Extents.z + pDesc->Extents.z)
	{
		m_fDepth = {};
		m_vMtv = {};
		return false;
	}

	// 가장 적게 겹치는 쪽으로 저장해둔다

	_float2 fOverlap = { m_pDesc->Extents.x + pDesc->Extents.x - fDistCenterToCenter.x , m_pDesc->Extents.z + pDesc->Extents.z - fDistCenterToCenter.y };

	// 위치 따라서 움직일 방향을 지정. 내가 밀리는 방향을 넣어둔다
	_float xSign = (m_pDesc->Center.x < pDesc->Center.x) ? -1.f : 1.f;
	_float zSign = (m_pDesc->Center.z < pDesc->Center.z) ? -1.f : 1.f;

	if (fOverlap.x <= fOverlap.y)
	{
		m_fDepth = fOverlap.x;
		m_vMtv = { fOverlap.x * xSign,0.f,0.f };
	}
	else
	{
		m_fDepth = fOverlap.y;
		m_vMtv = { 0.f,0.f,fOverlap.y * zSign };
	}


	return true;
}

_bool CBounding_AABB::Collision_AABB_To_OBB(CBounding* pTarget)
{
	const BoundingOrientedBox* pDesc = static_cast<CBounding_OBB*>(pTarget)->Get_Desc();


	// 값을 가져와서 행렬로 만들자
	_vector ObbCenterPos = XMLoadFloat3(&pDesc->Center);
	_vector ObbScale = XMLoadFloat3(&pDesc->Extents);
	_vector ObbRotation = XMLoadFloat4(&pDesc->Orientation);

	_matrix matScale = XMMatrixScalingFromVector(ObbScale * 2.0f);
	_matrix matRot = XMMatrixRotationQuaternion(ObbRotation);
	_matrix matTrans = XMMatrixTranslationFromVector(ObbCenterPos);

	// obb world를 만들어서 이걸로 이제 구를 가져오기
	_matrix matWorld = matScale * matRot * matTrans;

	// x,z 만 체크하려고
	auto Flatten = [](XMVECTOR v) {
		v.m128_f32[1] = 0.f; // y = 0
		return XMVector3Normalize(v);
		};


	// 축을 구한다, x,z만
	_vector vObbRight = (matWorld.r[0]);
	_vector vObbLook = (matWorld.r[2]);

	_vector vAxisX = { 1.f, 0.f, 0.f, 0.f };
	_vector vAxisZ = { 0.f, 0.f, 1.f, 0.f };

	
	vector<_vector> Axes = {
	Flatten(vObbRight),
	Flatten(vObbLook),
	vAxisX,
	vAxisZ,
	Flatten(XMVector3Cross(vAxisX, vObbRight)),
	Flatten(XMVector3Cross(vAxisX, vObbLook)),
	Flatten(XMVector3Cross(vAxisZ, vObbRight)),
	Flatten(XMVector3Cross(vAxisZ, vObbLook))
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
			fabsf(XMVectorGetX(XMVector3Dot(Axis, vAxisX))) * m_pDesc->Extents.x +
			fabsf(XMVectorGetX(XMVector3Dot(Axis, vAxisZ))) * m_pDesc->Extents.z;

		float projOBB =
			fabsf(XMVectorGetX(XMVector3Dot(Axis, vObbRight))) * pDesc->Extents.x +
			fabsf(XMVectorGetX(XMVector3Dot(Axis, vObbLook))) * pDesc->Extents.z;


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

_bool CBounding_AABB::Collision_AABB_To_Sphere(CBounding* pTarget)
{
	_bool isColl = static_cast<CBounding_Sphere*>(pTarget)->Collision_Sphere_To_AABB(this);

	Set_Depth(pTarget->Get_Depth());
	// 반대 방향으로 넣어주기
	_float3 f3Mtv = pTarget->Get_Mtv();
	_vector vMtv = XMLoadFloat3(&f3Mtv);
	XMStoreFloat3(&m_vMtv, XMVectorNegate(vMtv));


	return isColl;
}



#ifdef _DEBUG

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{

	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

#endif


_bool CBounding_AABB::Intersect_ToAABB(CBounding* pTarget)
{
	_float3		vSourMin = Compute_Min();
	_float3		vSourMax = Compute_Max();

	_float3		vDestMin = static_cast<CBounding_AABB*>(pTarget)->Compute_Min();
	_float3		vDestMax = static_cast<CBounding_AABB*>(pTarget)->Compute_Max();

	/* 너비상으로 겹쳤냐? */
	if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
		return false;

	if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
		return false;

	if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
		return false;

	return true;
}

_float3 CBounding_AABB::Compute_Min()
{
	return _float3(m_pDesc->Center.x - m_pDesc->Extents.x,
		m_pDesc->Center.y - m_pDesc->Extents.y,
		m_pDesc->Center.z - m_pDesc->Extents.z);
}

_float3 CBounding_AABB::Compute_Max()
{
	return _float3(m_pDesc->Center.x + m_pDesc->Extents.x,
		m_pDesc->Center.y + m_pDesc->Extents.y,
		m_pDesc->Center.z + m_pDesc->Extents.z);
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CBounding::BOUNDING_DESC* pDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);
}
