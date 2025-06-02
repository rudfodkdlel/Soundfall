#include "Camera_TopDown.h"
#include "GameInstance.h"

CCamera_TopDown::CCamera_TopDown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_TopDown::CCamera_TopDown(const CCamera_TopDown& Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CCamera_TopDown::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_TopDown::Initialize(void* pArg)
{
	CCamera::CAMERA_DESC			Desc{};

	Desc.vEye = _float3(0.f, 40.f, -30.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFov = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 500.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fSpeedPerSec = 10.0f;
	lstrcpy(Desc.szName, TEXT("Camera"));

	m_fSensor = 0.15f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	return S_OK;
}

void CCamera_TopDown::Priority_Update(_float fTimeDelta)
{
	// target 설정
	if (!m_IsTargeted)
	{
		m_pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));

		if(nullptr != m_pTarget)
			m_IsTargeted = true;
		// 못찾으면 return
	}

	


	__super::Bind_Matrices();



}

void CCamera_TopDown::Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;

	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	_vector vDir = vTargetPos - vPos;

	if (XMVectorGetZ(vDir) > 35.f)
	{
		vPos += {0.f, 0.f, fTimeDelta * 10.f, 0.f};
	}
	else if (XMVectorGetZ(vDir) < 25.f)
	{
		vPos -= {0.f, 0.f, fTimeDelta * 10.f, 0.f};
	}

	if (XMVectorGetX(vDir) > 5.f)
	{
		vPos += {fTimeDelta * 10.f, 0.f, 0.f, 0.f};
	}
	else if (XMVectorGetX(vDir) < -5.f)
	{
		vPos -= {fTimeDelta * 10.f, 0.f, 0.f, 0.f};
	}
	m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

void CCamera_TopDown::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_TopDown::Render()
{
	return S_OK;
}

CCamera_TopDown* CCamera_TopDown::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_TopDown* pInstance = new CCamera_TopDown(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_TopDown");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_TopDown::Clone(void* pArg)
{
	CCamera_TopDown* pInstance = new CCamera_TopDown(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_TopDown");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_TopDown::Free()
{
	__super::Free();

	Safe_Release(m_pTarget);
}
