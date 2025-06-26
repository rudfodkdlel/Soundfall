#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_Sphere::Initialize(const CBounding::BOUNDING_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
	const SPHERE_DESC* pSphereDesc = static_cast<const SPHERE_DESC*>(pDesc);

	m_pOriginalDesc = new BoundingSphere(pSphereDesc->vCenter, pSphereDesc->fRadius);
	m_pDesc = new BoundingSphere(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{

	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_Sphere::Intersect(CBounding* pTarget)
{
	_bool		isColl = { false };

	switch (pTarget->Get_Type())
	{
	case COLLIDER::AABB:
		isColl = Collision_Sphere_To_AABB(pTarget);
		//isColl = m_pDesc->Intersects(*static_cast<CBounding_AABB*>(pTarget)->Get_Desc());
		break;
	case COLLIDER::OBB:
		isColl = Collision_Sphere_To_OBB(pTarget);
		//isColl = m_pDesc->Intersects(*static_cast<CBounding_OBB*>(pTarget)->Get_Desc());
		break;
	case COLLIDER::SPHERE:
		isColl = Collision_Sphere_To_Sphere(pTarget);
		//isColl = m_pDesc->Intersects(*static_cast<CBounding_Sphere*>(pTarget)->Get_Desc());
		break;

	}

	return isColl;
}

_bool CBounding_Sphere::Collision_Sphere_To_Sphere(CBounding* pTarget)
{
	const BoundingSphere* pDesc = static_cast<CBounding_Sphere*>(pTarget)->Get_Desc();

	_float fLength = pDesc->Radius + m_pDesc->Radius;

	_vector vDir = XMLoadFloat3(&m_pDesc->Center) - XMLoadFloat3(&pDesc->Center);

	vDir.m128_f32[1] = 0;

	if (XMVectorGetX(XMVector3Length(vDir)) > fLength)
	{
		m_fDepth = {};
		m_vMtv = {};
		// 충돌 안했음
		return false;
	}

	m_fDepth = fLength - XMVectorGetX(XMVector3Length(vDir));

	if (m_fDepth < 0.00001f)
	{
		m_fDepth = {};
		m_vMtv = {};
		return false;
	}


	XMStoreFloat3(&m_vMtv, m_fDepth * XMVector3Normalize(vDir));


	return true;
}

_bool CBounding_Sphere::Collision_Sphere_To_AABB(CBounding* pTarget)
{
	const BoundingBox* pDesc = static_cast<CBounding_AABB*>(pTarget)->Get_Desc();

	_float3 fClosestPoint = {};

	// Y값 없애기 위해
	fClosestPoint.y = m_pDesc->Center.y;
	
	// 순서 신경쓰기. value - min - max 순으로
	fClosestPoint.x = clamp(m_pDesc->Center.x, pDesc->Center.x - pDesc->Extents.x, pDesc->Center.x + pDesc->Extents.x);
	fClosestPoint.z = clamp(m_pDesc->Center.z, pDesc->Center.z - pDesc->Extents.z, pDesc->Center.z + pDesc->Extents.z);

	float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pDesc->Center) - XMLoadFloat3(&fClosestPoint)));

	// 크면 충돌 안함
	if (fDist > m_pDesc->Radius)
	{
		m_fDepth = {};
		m_vMtv = {};
		return false;
	}

	m_fDepth = m_pDesc->Radius - fDist;

	// 크기 작으면 무시하기. 값은 적절히 조절해서 안끼는 느낌 나도록
	if (m_fDepth < 1e-6f)
	{
		m_fDepth = {};
		m_vMtv = {};
		return false;
	}

	_vector vDir = XMLoadFloat3(&m_pDesc->Center) - XMLoadFloat3(&fClosestPoint);

	XMStoreFloat3(&m_vMtv, m_fDepth * XMVector3Normalize(vDir));

	return true;
}

_bool CBounding_Sphere::Collision_Sphere_To_OBB(CBounding* pTarget)
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

	_matrix matWorldInv = XMMatrixInverse(nullptr, matWorld);

	// 구의 중심 좌표를 obb local로 옮기기
	_vector vSphereLocalCenter = XMVector3TransformCoord(XMLoadFloat3(&m_pDesc->Center), matWorldInv);

	_float3 fClosestPoint = {};

	fClosestPoint.y = vSphereLocalCenter.m128_f32[1];

	// 순서 신경쓰기. value - min - max 순으로
	fClosestPoint.x = clamp(vSphereLocalCenter.m128_f32[0],  - pDesc->Extents.x,  + pDesc->Extents.x);
	fClosestPoint.z = clamp(vSphereLocalCenter.m128_f32[2],  - pDesc->Extents.z,  + pDesc->Extents.z);

	float fDist = XMVectorGetX(XMVector3Length(vSphereLocalCenter - XMLoadFloat3(&fClosestPoint)));

	// 크면 충돌 안함
	if (fDist > m_pDesc->Radius)
	{
		m_fDepth = {};
		m_vMtv = {};
		return false;
	}

	m_fDepth = m_pDesc->Radius - fDist;

	// 크기 작으면 무시하기. 값은 적절히 조절해서 안끼는 느낌 나도록
	if (m_fDepth < 0.00001f)
	{
		m_fDepth = {};
		m_vMtv = {};
		return false;
	}

	_vector vDir = vSphereLocalCenter - XMLoadFloat3(&fClosestPoint);

	XMStoreFloat3(&m_vMtv, m_fDepth * XMVector3Normalize(vDir));

	return true;


}


#ifdef _DEBUG



HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{

	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

#endif


CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CBounding::BOUNDING_DESC* pDesc)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);
}
