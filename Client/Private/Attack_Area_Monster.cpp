#include "Attack_Area_Monster.h"
#include "GameInstance.h"

CAttack_Area_Monster::CAttack_Area_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice, pContext }
{
}

CAttack_Area_Monster::CAttack_Area_Monster(const CAttack_Area_Monster& Prototype)
    :CPartObject{ Prototype }
{
}

HRESULT CAttack_Area_Monster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAttack_Area_Monster::Initialize(void* pArg)
{
    HPBAR_DESC* pDesc = static_cast<HPBAR_DESC*>(pArg);


    m_pCombatCom = pDesc->pCombatCom;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Scaling(6.f, 6.f, 2.f);

    _vector vDir = { 1.f, 0.f,0.f,0.f };
    m_pTransformCom->Rotation(vDir, XMConvertToRadians(90.f));


    _vector vPos = { 0.f,0.1f + m_pGameInstance->Compute_Random_Normal() * 0.005f,m_pTransformCom->Get_Scaled().y / 2.f,0.f};
    vPos += m_pTransformCom->Get_State(STATE::POSITION);
    m_pTransformCom->Set_State(STATE::POSITION, vPos);



    return S_OK;
}

void CAttack_Area_Monster::Priority_Update(_float fTimeDelta)
{
}

void CAttack_Area_Monster::Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    if(m_fRatio < 3.f)
       m_fRatio += 3.f * fTimeDelta; 

   
    _vector vOriginPos = m_pTransformCom->Get_State(STATE::POSITION);

    m_pTransformCom->Scaling(6.f * m_fRatio, 6.f * m_fRatio, 2.f);

    
    float deltaZ = 6.f * m_fRatio  / 2.f;

    
    vOriginPos.m128_f32[2] = deltaZ;
    m_pTransformCom->Set_State(STATE::POSITION, vOriginPos);
}

void CAttack_Area_Monster::Late_Update(_float fTimeDelta)
{
    XMFLOAT4X4 parentMatrix = *m_pParentMatrix;

    XMStoreFloat4x4(
        &m_CombinedWorldMatrix,
        XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * XMLoadFloat4x4(&parentMatrix)
    );

   // Billboarding();
    if(m_isActive)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_BLEND, this);
}

HRESULT CAttack_Area_Monster::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _vector m_vColor = { 1.f,0.f,0.f,0.5f };

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vProjectileColor", &m_vColor, sizeof(m_vColor))))
        return E_FAIL;
    
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(4)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;



    return S_OK;
}

HRESULT CAttack_Area_Monster::Ready_Components()
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
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sniper_Range"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CAttack_Area_Monster::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CAttack_Area_Monster* CAttack_Area_Monster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAttack_Area_Monster* pInstance = new CAttack_Area_Monster(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMonster_HP");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAttack_Area_Monster::Clone(void* pArg)
{
    CAttack_Area_Monster* pInstance = new CAttack_Area_Monster(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CAttack_Area_Monster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAttack_Area_Monster::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
