#include "Tentacle_Melee.h"
#include "GameInstance.h"
#include "Model.h"

CTentacle_Melee::CTentacle_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTentacle_Melee::CTentacle_Melee(const CTentacle_Melee& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTentacle_Melee::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTentacle_Melee::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vPos));

	m_pModelCom->Set_Animation(7, false);

	return S_OK;
}

void CTentacle_Melee::Priority_Update(_float fTimeDelta)
{
	// 죽는거 판정

	if (STATE_MAIN::DEAD == m_eState && m_IsFinished)
		Set_Dead();

	// target 설정
	if (!m_IsTargeted)
	{
		m_pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));

		if (nullptr != m_pTarget)
			m_IsTargeted = true;
		// 못찾으면 return
	}
}

void CTentacle_Melee::Update(_float fTimeDelta)
{

	if (m_IsTargeted)
	{
		m_pTransformCom->LookAt(m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
	}

	m_IsFinished = m_pModelCom->Play_Animation(fTimeDelta);
}

void CTentacle_Melee::Late_Update(_float fTimeDelta)
{
	
	Select_State();

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CTentacle_Melee::Render()
{
	if (FAILED(Bind_ShaderResources()))
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

void CTentacle_Melee::Select_State()
{
	// 체력조건 만들어서 미리 보내버리기

	if (STATE_MAIN::DEAD == m_eState)
		return;

	if (m_IsFinished)
	{
		switch (m_eState)
		{
		case Client::STATE_MAIN::IDLE:
			m_eState = STATE_MAIN::ATTACK;
			m_pModelCom->Set_Animation(0, false);

			// 공격 받으면 이제 idle에서만 바꾸기
			
			break;
		case Client::STATE_MAIN::ATTACK:
			m_eState = STATE_MAIN::IDLE;
			m_pModelCom->Set_Animation(5, false);
			break;
		case Client::STATE_MAIN::HIT:
			break;
		case Client::STATE_MAIN::DEAD:
			// 체력으로 판정
			m_pModelCom->Set_Animation(6, false);
			break;
		case Client::STATE_MAIN::SPWAN:
			m_eState = STATE_MAIN::IDLE;
			m_pModelCom->Set_Animation(5, false);
			break;
		default:
			break;
		}
	}
}

HRESULT CTentacle_Melee::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Tentacle_Melee"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTentacle_Melee::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_Light(0);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CTentacle_Melee* CTentacle_Melee::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTentacle_Melee* pInstance = new CTentacle_Melee(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTentacle_Melee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTentacle_Melee::Clone(void* pArg)
{
	CTentacle_Melee* pInstance = new CTentacle_Melee(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTentacle_Melee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTentacle_Melee::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTarget);
}
