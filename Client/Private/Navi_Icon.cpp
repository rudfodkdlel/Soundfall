#include "Navi_Icon.h"
#include "GameInstance.h"

CNavi_Icon::CNavi_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CNavi_Icon::CNavi_Icon(const CNavi_Icon& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CNavi_Icon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNavi_Icon::Initialize(void* pArg)
{
	NAVI_UI_DESC* pDesc = static_cast<NAVI_UI_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_strCaption = pDesc->strCaption;

	m_isClick = false;

	return S_OK;
}

void CNavi_Icon::Priority_Update(_float fTimeDelta)
{
}

void CNavi_Icon::Update(_float fTimeDelta)
{
	
}

void CNavi_Icon::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_UI, this);
}

HRESULT CNavi_Icon::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (m_isClick)
	{
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;
	}
	

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	
	m_pGameInstance->Draw_Font(TEXT("Default"), m_strCaption.c_str(), _float2(m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f), XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, _float2(0.f, 0.f), 1.f);

	return S_OK;
}

_bool CNavi_Icon::Check_Click()
{
	POINT ptScreen;
	GetCursorPos(&ptScreen);

	// 항상 내 게임 윈도우 기준으로 좌표 변환해야 함
	ScreenToClient(g_hWnd, &ptScreen);

	
	

	if (ptScreen.x < m_fX - 0.5f * m_fSizeX || ptScreen.x >m_fX + 0.5f * m_fSizeX)
		return false;

	if (ptScreen.y < m_fY - 0.5f * m_fSizeY || ptScreen.y > m_fY + 0.5f * m_fSizeY)
		return false;

	return true;
}

HRESULT CNavi_Icon::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Navi_Button"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CNavi_Icon* CNavi_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavi_Icon* pInstance = new CNavi_Icon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNavi_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNavi_Icon::Clone(void* pArg)
{
	CNavi_Icon* pInstance = new CNavi_Icon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavi_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavi_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}

