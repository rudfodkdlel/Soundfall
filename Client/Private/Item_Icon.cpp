#include "Item_Icon.h"
#include "GameInstance.h"

CItem_Icon::CItem_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CItem_Icon::CItem_Icon(const CItem_Icon& Prototype)
	: CUIObject{ Prototype }
{
}

WEAPON_ICON_INFO_DESC CItem_Icon::Get_Weapon_Info_Desc()
{
	WEAPON_ICON_INFO_DESC eDesc = {};

	eDesc.iTextureType = m_iTextureType;

	return eDesc;
}

HRESULT CItem_Icon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_Icon::Initialize(void* pArg)
{
	ITEM_UI_DESC* pDesc = static_cast<ITEM_UI_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iTextureType = pDesc->iTextureType;

	m_isClick = false;

	return S_OK;
}

void CItem_Icon::Priority_Update(_float fTimeDelta)
{
}

void CItem_Icon::Update(_float fTimeDelta)
{
}

void CItem_Icon::Late_Update(_float fTimeDelta)
{
	if(m_isRender)
		m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_UI, this);
}

HRESULT CItem_Icon::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureType)))
		return E_FAIL;

	if (FAILED(m_pHighLightTextureCom->Bind_ShaderResource(m_pShaderCom, "g_HighLightTexture", 0)))
		return E_FAIL;

	if (!m_isClick)
	{
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(7)))
			return E_FAIL;
	}


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

void CItem_Icon::Change_Position(_float fX, _float fY)
{
	m_fX = fX;
	m_fY = fY;

	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - ViewportDesc.Width * 0.5f, -m_fY + ViewportDesc.Height * 0.5f, 0.f, 1.f));
}



_bool CItem_Icon::Check_Click()
{
	POINT ptScreen;
	GetCursorPos(&ptScreen);

	// 항상 내 게임 윈도우 기준으로 좌표 변환해야 함
	ScreenToClient(g_hWnd, &ptScreen);


	if (ptScreen.x < m_fX - 0.5f * m_fSizeX || ptScreen.x >m_fX + 0.5f * m_fSizeX)
		return false;

	if (ptScreen.y < m_fY - 0.5f * m_fSizeY || ptScreen.y > m_fY + 0.5f * m_fSizeY)
		return false;

	m_pGameInstance->StopSound(SOUND_PLAYER_EFFECT);
	m_pGameInstance->PlaySound(TEXT("NPC_Element_Impact_Vocal_Click_0.wav"), SOUND_PLAYER_EFFECT, 0.6f);

	return true;
}

HRESULT CItem_Icon::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Weapon_Icon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Item_Highlight"),
		TEXT("Com_Texture_Highlight"), reinterpret_cast<CComponent**>(&m_pHighLightTextureCom))))
		return E_FAIL;

	return S_OK;
}


CItem_Icon* CItem_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Icon* pInstance = new CItem_Icon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Icon::Clone(void* pArg)
{
	CItem_Icon* pInstance = new CItem_Icon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pHighLightTextureCom);
}

