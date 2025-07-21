#include "Monster_HP.h"
#include "GameInstance.h"

CMonster_HP::CMonster_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{pDevice, pContext}
{
}

CMonster_HP::CMonster_HP(const CMonster_HP& Prototype)
    :CPartObject{Prototype}
{
}

HRESULT CMonster_HP::Initialize_Prototype()
{


    return S_OK;
}

HRESULT CMonster_HP::Initialize(void* pArg)
{
    HPBAR_DESC* pDesc = static_cast<HPBAR_DESC*>(pArg);


    m_pCombatCom = pDesc->pCombatCom;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    _vector vPos = { 0.f,8.f,0.f,1.f };

    m_pTransformCom->Set_State(STATE::POSITION, vPos);

    m_pTransformCom->Scaling(4.f, 0.6f, 1.f);

    

    return S_OK;
}

void CMonster_HP::Priority_Update(_float fTimeDelta)
{
}

void CMonster_HP::Update(_float fTimeDelta)
{
   
}

void CMonster_HP::Late_Update(_float fTimeDelta)
{
    XMFLOAT4X4 parentMatrix = *m_pParentMatrix;

   
    parentMatrix._11 = 1.0f; parentMatrix._12 = 0.0f; parentMatrix._13 = 0.0f;
    parentMatrix._21 = 0.0f; parentMatrix._22 = 1.0f; parentMatrix._23 = 0.0f;
    parentMatrix._31 = 0.0f; parentMatrix._32 = 0.0f; parentMatrix._33 = 1.0f;
    // 위치는 유지
    // parentMatrix._41, _42, _43 는 그대로 둠

    // 결합
    XMStoreFloat4x4(
        &m_CombinedWorldMatrix,
        XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * XMLoadFloat4x4(&parentMatrix)
    );

   // Billboarding();

    m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONLIGHT, this);
}

HRESULT CMonster_HP::Render()
{
   

    if(FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(2)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

   

    return S_OK;

    
}

HRESULT CMonster_HP::Ready_Components()
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
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Metronome_Bar"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_HP::Bind_ShaderResources()
{
    CCombatStat::HPBUFFER HpBuffer = m_pCombatCom->Get_HP_BUFFER();

    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_HpBuffer", &HpBuffer, sizeof(CCombatStat::HPBUFFER))))
        return E_FAIL;


    if(FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CMonster_HP* CMonster_HP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster_HP* pInstance = new CMonster_HP(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMonster_HP");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster_HP::Clone(void* pArg)
{
    CMonster_HP* pInstance = new CMonster_HP(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMonster_HP");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMonster_HP::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
