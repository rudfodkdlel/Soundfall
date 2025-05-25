#pragma once
#include "Player.h"

#include "GameInstance.h"
#include "Model.h"
#include "Player_State_Idle.h"
#include "Player_State_Move.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};

	Desc.fRotationPerSec = 90.f;
	Desc.fSpeedPerSec = 10.f;
	lstrcpy(Desc.szName, TEXT("Player"));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 1. 서로 다른 애니메이션을 셋팅했음에도 같은 동작이 재생된다. : 뼈가 공유되기때문에. */
	/* 2. 같은 애니메이션을 셋했다면 재생속도가 빨라진다. : */

	m_pModelCom->Set_Animation(m_iAnimnum, true);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
}

void CPlayer::Update(_float fTimeDelta)
{

	Input_Key(fTimeDelta);



	if (nullptr != m_pState)
		m_pState->Update(this, fTimeDelta);

	
	Look_Mouse();

	bool a = m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, 1, 0)))
			return E_FAIL;

		m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);




		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}



	return S_OK;
}

void CPlayer::Input_Key(_float fTimeDelta)
{
	
	m_vDir = { 0.f,0.f,0.f,0.f };

	if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_vDir += {-1.f, 0.f, 0.f, 0.f};
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_vDir += {1.f, 0.f, 0.f, 0.f};
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_vDir += {0.f, 0.f, 1.f, 0.f};
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_vDir += {0.f, 0.f, -1.f, 0.f};
	}
	
	XMVector3Normalize(m_vDir);

	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

	// speed로 변경 필요
	vPos = vPos + m_vDir * 10.f * fTimeDelta;

	m_pTransformCom->Set_State(STATE::POSITION, vPos);
	
	if (XMVector3Equal(m_vDir, XMVectorZero()))
	{
		// Idle 상태로 전환
		if (nullptr == m_pState || dynamic_cast<Player_State_Idle*>(m_pState) == nullptr)
		{
			if (nullptr != m_pState)
			{
				m_pState->Exit(this);
				delete m_pState;
			}
			m_pState = new Player_State_Idle;
			m_pState->Enter(this);
		}
	}
	else
	{
		if (nullptr == m_pState || dynamic_cast<Player_State_Move*>(m_pState) == nullptr)
		{
			if (nullptr != m_pState)
			{
				m_pState->Exit(this);
				delete m_pState;
			}
			m_pState = new Player_State_Move;
			m_pState->Enter(this);
		}
	}

	//


}

void CPlayer::Look_Mouse()
{
	_float4 MousePos = {};
	_vector vPlanePoint = { 0.f, 0.f, 0.f, 1.f };
	_vector vDir = { 0.f, 1.f, 0.f, 0.f };
	MousePos = m_pGameInstance->Get_Mouse_Projection(vPlanePoint, vDir);

	if (MousePos.w != 0.f)
		m_pTransformCom->LookAt(XMLoadFloat4(&MousePos));
}

DIR_STATE CPlayer::Calc_Dir()
{
	
	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
	vLook = XMVector3Normalize(vLook);

	if (XMVector3Equal(m_vDir, XMVectorZero()))
		return NONE;

	_float dot = XMVectorGetX(XMVector3Dot(m_vDir, vLook));
	_float x = XMVectorGetX(m_vDir);

	// 기준 각도 값
	const _float COS_45 = 0.707f;
	const _float COS_135 = -0.707f;

	// 앞
	if (dot >= COS_45)
	{
		if (x > 0)      return FR;
		else if (x < 0) return FL;
		else            return F;
	}
	// 좌우
	else if (dot > COS_135)
	{
		if (x > 0)      return R;
		else if (x < 0) return L;
	}
	// 뒤
	else
	{
		if (x > 0)      return BR;
		else if (x < 0) return BL;
		else            return B;
	}


}

HRESULT CPlayer::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Ky"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

	if (nullptr != m_pState)
		delete m_pState;
}
