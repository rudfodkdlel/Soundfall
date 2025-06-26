#include "Hit_Effect_Texture.h"
#include "GameInstance.h"

CHit_Effect_Texture::CHit_Effect_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CHit_Effect_Texture::CHit_Effect_Texture(const CHit_Effect_Texture& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CHit_Effect_Texture::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHit_Effect_Texture::Initialize(void* pArg)
{


    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    _vector vPos = XMLoadFloat4(&pDesc->vPos);
    vPos.m128_f32[1] += 5.f;

    m_pTransformCom->Set_State(STATE::POSITION, vPos);

    m_pTransformCom->Scaling(3.f, 3.f, 3.f);

    return S_OK;
}

void CHit_Effect_Texture::Priority_Update(_float fTimeDelta)
{
    if (m_fDuration < 0.f)
        Set_Dead();
}

void CHit_Effect_Texture::Update(_float fTimeDelta)
{
    m_fDuration -= fTimeDelta;
    __super::Billboarding();
}

void CHit_Effect_Texture::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CHit_Effect_Texture::Render()
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

HRESULT CHit_Effect_Texture::Ready_Components()
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
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Hit_Effect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    return S_OK;
}

CHit_Effect_Texture* CHit_Effect_Texture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHit_Effect_Texture* pInstance = new CHit_Effect_Texture(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHit_Effect_Texture");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHit_Effect_Texture::Clone(void* pArg)
{
    CHit_Effect_Texture* pInstance = new CHit_Effect_Texture(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHit_Effect_Texture");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHit_Effect_Texture::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}

