#include "LoadingIcon.h"

#include "GameInstance.h"


CLoadingIcon::CLoadingIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CLoadingIcon::CLoadingIcon(const CLoadingIcon& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CLoadingIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLoadingIcon::Initialize(void* pArg)
{
    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

	pDesc->fRotationPerSec = XMConvertToRadians(90.f);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    

    return S_OK;
}

void CLoadingIcon::Priority_Update(_float fTimeDelta)
{
}

void CLoadingIcon::Update(_float fTimeDelta)
{
	
    // 회전 기능 추가

	_vector vAxis = { 0.f,0.f,1.f,0.f };
	
	m_pTransformCom->Turn(vAxis, fTimeDelta);
	
		

}

void CLoadingIcon::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_PRIORITY, this);
}

HRESULT CLoadingIcon::Render()
{
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE; // <- Culling 끄는 부분
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthClipEnable = true;

	ID3D11RasterizerState* pRasterState = nullptr;
	HRESULT hr = m_pDevice->CreateRasterizerState(&rasterDesc, &pRasterState);
	m_pContext->RSSetState(pRasterState);

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

	rasterDesc.CullMode = D3D11_CULL_BACK;

	pRasterState->Release();

	return S_OK;
}

HRESULT CLoadingIcon::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LodingIcon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
}

CLoadingIcon* CLoadingIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoadingIcon* pInstance = new CLoadingIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoadingIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoadingIcon::Clone(void* pArg)
{
	CLoadingIcon* pInstance = new CLoadingIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoadingIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadingIcon::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
