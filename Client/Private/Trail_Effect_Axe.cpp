#include "Trail_Effect_Axe.h"
#include "GameInstance.h"

CTrail_Effect_Axe::CTrail_Effect_Axe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{pDevice, pContext}
{
}

CTrail_Effect_Axe::CTrail_Effect_Axe(const CTrail_Effect_Axe& Prototype)
    :CGameObject{Prototype}
{
}

HRESULT CTrail_Effect_Axe::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrail_Effect_Axe::Initialize(void* pArg)
{
	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	TRAIL_AXE_DESC* pTrailDesc = static_cast<TRAIL_AXE_DESC*>(pArg);

	m_vDir = pTrailDesc->vDir;
	m_vColor = pTrailDesc->vColor;

	float yaw = atan2f(XMVectorGetX(XMLoadFloat4(&m_vDir)), XMVectorGetZ(XMLoadFloat4(&m_vDir)));

	_vector vQuaternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90.f), yaw, 0.0f);

	_matrix matRotaion = XMMatrixRotationQuaternion(vQuaternion);

	XMStoreFloat4x4(m_pTransformCom->Get_WorldMatrix(), matRotaion * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));

	_vector vPos = XMLoadFloat4(&pDesc->vPos);

	vPos.m128_f32[1] += 2.f;

	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	m_pTransformCom->Scaling(15.f, 15.f, 1.f);

	return S_OK;
}


void CTrail_Effect_Axe::Priority_Update(_float fTimeDelta)
{
	//
}

void CTrail_Effect_Axe::Update(_float fTimeDelta)
{
	//
}

void CTrail_Effect_Axe::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONLIGHT, this);
}

HRESULT CTrail_Effect_Axe::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	// 패스는 생각좀
	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG

	//m_pColliderCom->Render();

#endif

	return S_OK;
}

HRESULT CTrail_Effect_Axe::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Axe_Effect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CTrail_Effect_Axe* CTrail_Effect_Axe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail_Effect_Axe* pInstance = new CTrail_Effect_Axe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrail_Effect_Axe");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrail_Effect_Axe::Clone(void* pArg)
{
	CTrail_Effect_Axe* pInstance = new CTrail_Effect_Axe(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrail_Effect_Axe");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Effect_Axe::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}

