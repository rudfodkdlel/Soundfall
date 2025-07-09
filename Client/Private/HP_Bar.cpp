#include "HP_Bar.h"
#include "GameInstance.h"
#include "CombatStat.h"

CHP_BAR::CHP_BAR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CHP_BAR::CHP_BAR(const CHP_BAR& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CHP_BAR::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHP_BAR::Initialize(void* pArg)
{
    HPBARDESC* hpDesc = static_cast<HPBARDESC*>(pArg);
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_IsPlayer = hpDesc->IsPlayer;

    return S_OK;
}

void CHP_BAR::Priority_Update(_float fTimeDelta)
{
    // target 설정
    if (!m_IsTargeted)
    {
        if (m_IsPlayer)
        {
            m_pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));

            if (nullptr != m_pTarget)
                m_IsTargeted = true;
            // 못찾으면 return

            m_pCombatCom = static_cast<CCombatStat*>(m_pTarget->Get_Component(TEXT("Com_Combat")));
        }
        else
        {
            m_pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Boss"));

            if (nullptr != m_pTarget)
                m_IsTargeted = true;
            // 못찾으면 return

            m_pCombatCom = static_cast<CCombatStat*>(m_pTarget->Get_Component(TEXT("Com_Combat")));
        }
       
    }
}

void CHP_BAR::Update(_float fTimeDelta)
{
    if (nullptr == m_pTarget && m_IsTargeted)
        Set_Dead();
}

void CHP_BAR::Late_Update(_float fTimeDelta)
{
    if (nullptr != m_pCombatCom)
    {
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_UI, this);
    }
}

HRESULT CHP_BAR::Render()
{
   
    CCombatStat::HPBUFFER HpBuffer = m_pCombatCom->Get_HP_BUFFER();


    if (FAILED(m_pShaderCom->Bind_RawValue("g_HpBuffer", &HpBuffer, sizeof(CCombatStat::HPBUFFER))))
        return E_FAIL;
    
       

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (m_IsPlayer)
    {
        if (FAILED(m_pTextureIdleCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        if (FAILED(m_pTextureDamagedCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Add", 0)))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pTextureIdleCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Add", 0)))
            return E_FAIL;

        if (FAILED(m_pTextureDamagedCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;
    }
   


    if (FAILED(m_pShaderCom->Begin(3)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (m_IsPlayer)
    {
        wstring str = std::to_wstring(m_pCombatCom->Get_Current_HP()) + std::wstring(TEXT(" / ")) + to_wstring(m_pCombatCom->Get_Max_HP());

        m_pGameInstance->Draw_Font(TEXT("Default"), str.c_str(), _float2(225, g_iWinSizeY * 0.735f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 0.5f);
    }

    return S_OK;
}

HRESULT CHP_BAR::Ready_Components()
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
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_HP_Idle"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureIdleCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_HP_Damaged"),
        TEXT("Com_Texture_Add"), reinterpret_cast<CComponent**>(&m_pTextureDamagedCom))))
        return E_FAIL;

    return S_OK;
}

CHP_BAR* CHP_BAR::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHP_BAR* pInstance = new CHP_BAR(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHP_BAR");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHP_BAR::Clone(void* pArg)
{
    CHP_BAR* pInstance = new CHP_BAR(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHP_BAR");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHP_BAR::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureIdleCom);
    Safe_Release(m_pTextureDamagedCom);
}
