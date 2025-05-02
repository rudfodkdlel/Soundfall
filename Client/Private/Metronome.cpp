#include "Metronome.h"
#include "Metronome_Counter.h"
#include "GameInstance.h"


CMetronome::CMetronome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CMetronome::CMetronome(const CMetronome& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CMetronome::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMetronome::Initialize(void* pArg)
{
	METRONOME_DESC* pDesc = static_cast<METRONOME_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CMetronome::Priority_Update(_float fTimeDelta)
{
}

void CMetronome::Update(_float fTimeDelta)
{

	if (m_iNumCount == 6)
		return;
	m_fSpawnTime += fTimeDelta;

	
	while (m_iNumCount < 6 && m_fSpawnTime >= m_fSpawnInterval * m_iNumCount / 2.f -  m_fSpawnInterval * 0.5f)
	
	{
		CMetronome_Counter::METRONOME_COUNTER_DESC			countDesc{};

		countDesc.fX = g_iWinSizeX * 0.5f - 200.f;
		countDesc.fY = g_iWinSizeY * 0.5f + 200.f;
		countDesc.fSizeX = 1.f;
		countDesc.fSizeY = 50.f;
		countDesc.fSpeedPerSec = 200.f / m_fSpawnInterval;
		countDesc.vDir = _float3(1.f, 0.f, 0.f);
		countDesc.fStartPoint = countDesc.fX;

		if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::LEVEL_STATIC), TEXT("Prototype_GameObject_Metronome_Counter"),
			static_cast<_uint>(LEVEL::LEVEL_GAMEPLAY), TEXT("Layer_Metronome_Counter"), &countDesc)))
			return;


		countDesc.fX = g_iWinSizeX * 0.5f + 200.f;
		countDesc.vDir = _float3(-1.f, 0.f, 0.f);
		countDesc.fStartPoint = countDesc.fX;

		if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::LEVEL_STATIC), TEXT("Prototype_GameObject_Metronome_Counter"),
			static_cast<_uint>(LEVEL::LEVEL_GAMEPLAY), TEXT("Layer_Metronome_Counter"), &countDesc)))
			return;

		m_iNumCount += 2;

	}
}

void CMetronome::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_PRIORITY, this);
}

HRESULT CMetronome::Render()
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

HRESULT CMetronome::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_STATIC), TEXT("Prototype_Component_Texture_Metronome_Center_Anchor"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CMetronome* CMetronome::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMetronome* pInstance = new CMetronome(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMetronome");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMetronome::Clone(void* pArg)
{
	CMetronome* pInstance = new CMetronome(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMetronome");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMetronome::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
