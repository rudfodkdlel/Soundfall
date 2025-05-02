#include "Metronome_Counter.h"
#include "GameInstance.h"

CMetronome_Counter::CMetronome_Counter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CMetronome_Counter::CMetronome_Counter(const CMetronome_Counter& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CMetronome_Counter::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMetronome_Counter::Initialize(void* pArg)
{
	METRONOME_COUNTER_DESC* pDesc = static_cast<METRONOME_COUNTER_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSpeed = pDesc->fSpeedPerSec;
	m_vDir = pDesc->vDir;
	m_fStartPoint = pDesc->fStartPoint;

	return S_OK;
}

void CMetronome_Counter::Priority_Update(_float fTimeDelta)
{
	if (m_vDir.x > 0)
	{
		if (m_fX >= g_iWinSizeX * 0.5f)
		{
			m_fX = m_fStartPoint;
		}
	}
	else
	{
		if (m_fX <= g_iWinSizeX * 0.5f)
		{
			m_fX = m_fStartPoint;
		}
	}
}

void CMetronome_Counter::Update(_float fTimeDelta)
{
	m_fX += m_vDir.x * m_fSpeed * fTimeDelta;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
}

void CMetronome_Counter::Late_Update(_float fTimeDelta)
{


    m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_PRIORITY, this);
}

HRESULT CMetronome_Counter::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMetronome_Counter::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_STATIC), TEXT("Prototype_Component_Texture_Metronome_Counter"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CMetronome_Counter* CMetronome_Counter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMetronome_Counter* pInstance = new CMetronome_Counter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMetronome_Counter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMetronome_Counter::Clone(void* pArg)
{
	CMetronome_Counter* pInstance = new CMetronome_Counter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMetronome_Counter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMetronome_Counter::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
