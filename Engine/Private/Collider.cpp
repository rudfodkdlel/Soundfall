#include "Collider.h"

#include "GameInstance.h"


CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent{ Prototype }
	, m_eType{ Prototype.m_eType }
	, m_pBounding{ Prototype.m_pBounding }
	, m_pBatch{ Prototype.m_pBatch }
	, m_pEffect{ Prototype.m_pEffect }
#ifdef _DEBUG
	, m_pInputLayout{ Prototype.m_pInputLayout }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(COLLIDER eType)
{
	m_eType = eType;


#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = {  };

	m_pEffect->SetVertexColorEnabled(true);

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;



#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	CBounding::BOUNDING_DESC* pDesc = static_cast<CBounding::BOUNDING_DESC*>(pArg);

	pDesc->eType = m_eType;

	switch (m_eType)
	{
	case COLLIDER::AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pDesc);
		break;
	case COLLIDER::OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pDesc);
		break;
	case COLLIDER::SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pDesc);
		break;
	}

	return S_OK;
}

void CCollider::Update(_fmatrix WorldMatrix)
{
	m_pBounding->Update(WorldMatrix);
}

_bool CCollider::Intersect(CCollider* pTargetCollider)
{
	m_isColl = m_pBounding->Intersect(pTargetCollider->m_pBounding);

	if (m_isColl)
	{
		pTargetCollider->Set_IsColl(true);
	}

	return m_isColl;
}

_float4 CCollider::Calc_PushVector(CCollider* pTargetCollider)
{
	_float4 vPushVector = {};
	_float3 vCenter = Get_Center();
	_float3 vOtherCenter = pTargetCollider->Get_Center();
	_float3 myExtend = Get_Extends();
	_float3 otherExtend = pTargetCollider->Get_Extends();

	// 구, aabb, obb 3가지로 나눠서 구현 or 구 인지 구가 아닌지만 나눠서 

	_vector vDir = XMVectorSetW((XMLoadFloat3(&vCenter) - XMLoadFloat3(&vOtherCenter)), 0.f);
	vDir = XMVectorSetY(vDir, 0.f);

	_float fScale = XMVectorGetX(XMVector3Length(XMLoadFloat3(&myExtend)) + XMVector3Length(XMLoadFloat3(&otherExtend))) - XMVectorGetX(XMVector3Length(vDir));
	vDir = XMVector3Normalize(vDir);
	vDir = XMVectorScale(vDir, fabs(fScale));

	XMStoreFloat4(&vPushVector, vDir);
	return vPushVector;

	//나중에

	//if (COLLIDER::SPHERE == pTargetCollider->Get_Type())
	//{
	//	// 중점과 중점 을 이용해서 방향벡터를 만들어서 적용시킨다
	//	
	//	_float fScale = Get_Extends().x + pTargetCollider->Get_Extends().x - XMVectorGetX(XMVector3Length(vDir));
	//	vDir = XMVector3Normalize(vDir);
	//	vDir = XMVectorScale(vDir, fabs(fScale));
	//	
	//	XMStoreFloat4(&vPushVector, vDir);
	//}
	//else if(COLLIDER::AABB == pTargetCollider->Get_Type())
	//{
	//	// x랑 z만
	//	_vector axes[2] = {
	//	XMVectorSet(1.f, 0.f, 0.f, 0.f),
	//	XMVectorSet(0.f, 0.f, 1.f, 0.f)
	//	};

	//	float maxOverlap = -FLT_MAX;

	//	_float3 myExtend = Get_Extends();
	//	_float3 otherExtend = pTargetCollider->Get_Extends();

	//	_vector myExtends = XMLoadFloat3(&myExtend);
	//	_vector otherExtends = XMLoadFloat3(&otherExtend);

	//	for (int i = 0; i < 2; ++i)
	//	{
	//		// 축 방향 법선
	//		_vector axis = axes[i];

	//		// vDir과 축 벡터 내적 (절댓값)
	//		float dot = fabs(XMVectorGetX(XMVector3Dot(vDir, axis)));


	//		float radiusSum = XMVectorGetX(myExtends * axis) + XMVectorGetX(otherExtends * axis);
	//		float myRadius = 0.f;
	//		float otherRadius = 0.f;

	//		if (i == 0) // x축
	//		{
	//			myRadius = Get_Extends().x;
	//			otherRadius = pTargetCollider->Get_Extends().x;
	//		}
	//		else if (i == 1) // z축
	//		{
	//			myRadius = Get_Extends().z;
	//			otherRadius = pTargetCollider->Get_Extends().z;
	//		}

	//		radiusSum = myRadius + otherRadius;

	//		float overlap = radiusSum - dot;

	//		if (overlap > maxOverlap)
	//		{
	//			maxOverlap = overlap;

	//			// pushDir 방향은 vDir과 축 내적 부호 확인 후 축 방향 지정
	//			float sign = XMVectorGetX(XMVector3Dot(vDir, axis)) > 0 ? 1.f : -1.f;
	//			XMStoreFloat4(&vPushVector, axis * sign * overlap);
	//		}
	//	}
	//	
	//}
	//else if (COLLIDER::OBB == pTargetCollider->Get_Type())
	//{
	//	_float4 quat = static_cast<CBounding_OBB*>(pTargetCollider->Get_Bounding())->Get_Desc()->Orientation;

	//	_vector axes[2] = {
	//		XMVectorSet(1.f, 0.f, 0.f, 0.f),
	//		XMVectorSet(0.f, 0.f, 1.f, 0.f)
	//	};

	//	axes[0] = XMVector3Rotate(axes[0], XMLoadFloat4(&quat));
	//	axes[1] = XMVector3Rotate(axes[1], XMLoadFloat4(&quat));

	//	float maxOverlap = -FLT_MAX;

	//	_float3 myExtend = Get_Extends();
	//	_float3 otherExtend = pTargetCollider->Get_Extends();

	//	for (int i = 0; i < 2; ++i)
	//	{
	//		_vector axis = XMVector3Normalize(axes[i]);

	//		// 중심 간 방향 벡터 vDir를 이 축으로 투영 (projection length)
	//		float distOnAxis = fabs(XMVectorGetX(XMVector3Dot(vDir, axis)));

	//		// 각 콜라이더 반지름 (extend가 반지름 길이라고 가정)
	//		float myRadius = (i == 0) ? myExtend.x : myExtend.z;
	//		float otherRadius = (i == 0) ? otherExtend.x : otherExtend.z;

	//		float radiusSum = myRadius + otherRadius;

	//		float overlap = radiusSum - distOnAxis;

	//		if (overlap > maxOverlap)
	//		{
	//			maxOverlap = overlap;

	//			// push 방향은 vDir과 axis 내적 부호에 따라 결정
	//			float sign = (XMVectorGetX(XMVector3Dot(vDir, axis)) > 0) ? 1.f : -1.f;

	//			XMStoreFloat4(&vPushVector, axis * sign * overlap);
	//		}
	//	}


	//}


	return vPushVector;
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch, true == m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	m_pBatch->End();

	return S_OK;
}
#endif

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();



	Safe_Release(m_pBounding);

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif
}
