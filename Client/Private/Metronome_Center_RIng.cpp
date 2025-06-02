#include "Metronome_Center_RIng.h"
#include "GameInstance.h"

CMetronome_Center_RIng::CMetronome_Center_RIng(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CMetronome_Center_RIng::CMetronome_Center_RIng(const CMetronome_Center_RIng& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CMetronome_Center_RIng::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMetronome_Center_RIng::Initialize(void* pArg)
{
	UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(m_fScale, m_fScale, m_fScale);

	_vector vDir = { 1.f, 0.f,0.f,0.f };
	m_pTransformCom->Rotation(vDir, XMConvertToRadians(90.f));

	return S_OK;
}

void CMetronome_Center_RIng::Priority_Update(_float fTimeDelta)
{
	// target 설정
	if (!m_IsTargeted)
	{
		m_pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));

		if (nullptr != m_pTarget)
			m_IsTargeted = true;
		// 못찾으면 return
	}
}

void CMetronome_Center_RIng::Update(_float fTimeDelta)
{

	m_fTime += fTimeDelta;

	_float scaleX =  fabs(sin(XMConvertToRadians(m_fTime * 180.f))) + 1.f; // 0.5 ~ 1.0 사이
	_float scaleY =  fabs(sin(XMConvertToRadians(m_fTime * 180.f))) + 1.f;

	m_pTransformCom->Scaling(scaleX * m_fScale, scaleY * m_fScale, 1.f);

	_vector vPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	_vector vDir = XMVector3Normalize(m_pTarget->Get_Transform()->Get_State(STATE::LOOK));
	vDir = vDir * 4.f;

	vPos += {0.f, 0.31f, 0.f, 0.f};

	vPos += vDir;

	m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

void CMetronome_Center_RIng::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_PRIORITY, this);
}

HRESULT CMetronome_Center_RIng::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMetronome_Center_RIng::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Metronome_Center_Ring"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CMetronome_Center_RIng* CMetronome_Center_RIng::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMetronome_Center_RIng* pInstance = new CMetronome_Center_RIng(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMetronome_Center_RIng");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMetronome_Center_RIng::Clone(void* pArg)
{
	CMetronome_Center_RIng* pInstance = new CMetronome_Center_RIng(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMetronome_Center_RIng");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMetronome_Center_RIng::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
