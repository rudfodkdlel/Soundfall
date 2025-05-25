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

	Desc.vEye = _float3(0.f, 20.f, -15.f);
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

		// 못찾으면 return
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	_long			MouseMove = {};

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM::X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * m_fSensor);
	}

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM::Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), MouseMove * fTimeDelta * m_fSensor);
	}



	__super::Bind_Matrices();



}

void CCamera_TopDown::Update(_float fTimeDelta)
{
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
