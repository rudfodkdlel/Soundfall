#include "PlayerRing.h"
#include "GameInstance.h"

CPlayerRing::CPlayerRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CPlayerRing::CPlayerRing(const CPlayerRing& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CPlayerRing::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayerRing::Initialize(void* pArg)
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

void CPlayerRing::Priority_Update(_float fTimeDelta)
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

void CPlayerRing::Update(_float fTimeDelta)
{
    if (m_IsTargeted)
    {
        _vector vPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

        vPos += {0.f, 0.3f, 0.f, 0.f};

        m_pTransformCom->Set_State(STATE::POSITION, vPos);
    }
       
}

void CPlayerRing::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_UI, this);
}

HRESULT CPlayerRing::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
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

HRESULT CPlayerRing::Ready_Components()
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
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_Ring"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

CPlayerRing* CPlayerRing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayerRing* pInstance = new CPlayerRing(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayerRing");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerRing::Clone(void* pArg)
{
    CPlayerRing* pInstance = new CPlayerRing(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CPlayerRing");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerRing::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
